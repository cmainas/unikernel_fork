#include <sys/syscallargs.h>
#include <sys/filedesc.h>
#include <sys/file.h>
#include <sys/proc.h>
#include <sys/malloc.h>
#include <sys/bus.h> /* structs, prototypes for pci bus stuff and DEVMETHOD macros! */


#include "my_pipe.h"

int my_pipe_close(file_t *fp);
int my_pipe_read(file_t *fp, off_t *offset, struct uio *uio, kauth_cred_t cred,
		int flags);
int my_pipe_write(file_t *fp, off_t *offset, struct uio *uio, kauth_cred_t cred,
		int flags);

const struct fileops my_pipeops = {
	.fo_read = my_pipe_read,
	.fo_write = my_pipe_write,
	.fo_close = my_pipe_close,
};

/*
 * Handle the close request 
 */
int my_pipe_close(file_t *fp)
{
	struct my_pipe_op *op = fp->f_data; 
	struct my_pipe *pipe = op->pipe; 
	uint8_t nparts[2];
	fp->f_data = NULL;
	/* free my_pipe_op struct of process */
	free(op, M_TEMP);
	/* decrease number of writers or readers in pipe */
	pipe_lock(pipe->lock);
	read_region_1(pipe->nreaders, nparts, 2);
	if(op->oper == 0) 
		nparts[0]--;
	else if(op->oper == 1)
		nparts[1]--;
	write_region_1(pipe->nreaders, nparts, 2);
	pipe_unlock(pipe->lock);
	/* decrease number of writers or readers in pipe from current process */
	if(op->oper == 0) 
		pipe->pr_readers--;
	else if(op->oper == 1)
		pipe->pr_writers--;
	/* free my_pipe struct if no readers and writers exist */
	if(pipe->pr_readers == 0 && pipe->pr_writers == 0) 
		free(pipe, M_TEMP);
	/* clean used shared memory if no readers or writers exist */
	if(nparts[0] == 0 && nparts[1] == 0) 
		memset((void *)sharme.data_b, 0, 20 + MY_PIPE_BUF_SIZE);
	return 0; /*this always succeeds */
}

/*
 * Handle the read 
 */
int my_pipe_read(file_t *fp, off_t *offset, struct uio *uio, kauth_cred_t cred,
		int flags)
{
	struct my_pipe_op *op = fp->f_data; 
	struct my_pipe *pipe = op->pipe; 
	int ret = 0;
	size_t nread = 0, size;
	uint8_t nwriters;
	uint32_t bigs[4], cnt, len, in, out;
	/* keep trying until userspace gets as many bytes as it asked or until
	 * pipe gets empty*/
	while (uio->uio_resid) {
		/* wait until something is written in pipe, without acquiring
		 * the lock */
		cnt = bus_space_read_4(sharme.data_t, sharme.data_h, 
				pipe->cnt);
		while (cnt == 0) {
			/* if no writers exist then return EOF or the bytes
			 * that have been read until now */
			if (bus_space_read_1(sharme.data_t, sharme.data_h,
						pipe->nwriters) == 0) 
				break;
			cnt = bus_space_read_4(sharme.data_t, sharme.data_h, 
					pipe->cnt);
		}
		pipe_lock(pipe->lock);
		read_region_4(pipe->len, bigs, 4);
		len = bigs[0];
		in = bigs[1];
		out = bigs[2];
		cnt = bigs[3];
		printf("READ1: len=%d, cnt=%d, in=%d, out=%d\n", len, cnt, in, out);
		/* check with the lock if any data is available */
		if (cnt > 0) {
			/* determine the number of bytes that will be read */
			size = len - out;
			if (size > cnt)
				size = cnt;
			if (size > uio->uio_resid)
				size = uio->uio_resid;
			ret = uiomove((void *) (sharme.data_b + pipe->buf + out)
					, size, uio);
			if (ret) {
				pipe_unlock(pipe->lock);
				break;
			}
			/* update out pointer and number of bytes in pipe */
			out += size;
			if (out >= len)
				out = 0;
			cnt -= size;
			/* if all data has been read then start using the 
			 * pipe buffer from the beginning */
			if (cnt == 0) {
				in = 0;
				out = 0;
			}
			nread += size;
			bigs[1] = in;
			bigs[2] = out;
			bigs[3] = cnt;
			printf("READ2: len=%d, cnt=%d, in=%d, out=%d\n", len, cnt, in, out);
			write_region_4(pipe->len, bigs, 4);
		}
		nwriters = bus_space_read_1(sharme.data_t, sharme.data_h, 
				pipe->nwriters);
		pipe_unlock(pipe->lock);
		/* if no data is available then return what has been read */
		if (nread > 0 && cnt == 0) 
			break;
		if (nwriters == 0) 
			break;
	}
	return ret;
}

/*
 * Handle the write for the device
 */
int my_pipe_write(file_t *fp, off_t *offset, struct uio *uio, kauth_cred_t cred,
	       	int flags)
{
	struct my_pipe_op *op = fp->f_data; 
	struct my_pipe *pipe = op->pipe; 
	int ret = 0;
	size_t space;
	int size;
	uint8_t nreaders;
	uint32_t bigs[4], len, cnt, in;
	//read_region_4(pipe->len, bigs, 4);
	len = bus_space_read_4(sharme.data_t, sharme.data_h, pipe->len);
	cnt = bus_space_read_4(sharme.data_t, sharme.data_h, pipe->cnt);
	//len = bigs[0];
	//in = bigs[1];
	//out = bigs[2];
	//cnt = bigs[3];
	//printf("WRITE1: len=%d, cnt=%d, in =%d\n", len, cnt, in);
	/* get the lock for writing (atomic write) */
	pipe_lock(pipe->wr_lock);
	/* keep trying until all bytes are written in pipe, or until all the 
	 * readers leave */
	while (uio->uio_resid) {
		//space = len - cnt;
		/* wait for space, without acquiring the lock */
		do {
			/* if no readers exist then EPIPE must be returned */
			nreaders = bus_space_read_1(sharme.data_t, 
					sharme.data_h, 	pipe->nreaders);
			if (nreaders == 0)
				break;
			cnt = bus_space_read_4(sharme.data_t, sharme.data_h,
					pipe->cnt);
			space = len - cnt;
		} while (space == 0);
		pipe_lock(pipe->lock);
		read_region_4(pipe->len, bigs, 4);
		len = bigs[0];
		in = bigs[1];
		printf("WRITE2: len=%d, cnt=%d, in =%d\n", len, cnt, in);
		//out = bigs[2];
		cnt = bigs[3];
		/* check for space with the lock */
		space = len - cnt;
		if (space > 0 && nreaders != 0) {
			/* determine tha number of bytes tha will be written */
			if (space > uio->uio_resid)
				size = uio->uio_resid;
			else 
				size = space;
			if (size > (len - in))
				size = len - in;
			ret = uiomove((void *) (sharme.data_b + pipe->buf + in),
					size, uio);
			if(ret) {
				pipe_unlock(pipe->lock);
				break;
			}
			/* update in pointer and number of bytes in pipe */
			in += size;
			/* circular writing in buffer */
			if (in >= len)
				in = 0;
			cnt += size;
			bigs[1] = in;
			bigs[3] = cnt;
			printf("WRITE3: len=%d, cnt=%d, in =%d\n", len, cnt, in);
			write_region_4(pipe->len, bigs, 4);
		}
		nreaders = bus_space_read_1(sharme.data_t, sharme.data_h, 
				pipe->nreaders);
		pipe_unlock(pipe->lock);
		/* if no readers exist return EPIPE */
		if (nreaders == 0) {
			ret = EPIPE;
			break;
		}
	}
	pipe_unlock(pipe->wr_lock);
	return ret;
}

/*
 * Handle the system call
 */
int sys_my_pipe(struct lwp *l, const struct sys_my_pipe_args *uap, 
		register_t *retval)
{
	file_t *rf, *wf;
	int fd[2], error, descr;
	struct my_pipe *pipe = NULL;
	struct my_pipe_op *ro = NULL, *wo = NULL;
	uint8_t init; 
	uint8_t smalls[5], nparts[2];
	uint32_t bigs[4];

	/* allocate my_pipe_op and my_pipe structs and initialize them */
	pipe = malloc(sizeof(struct my_pipe), M_TEMP, M_WAITOK);
	if (pipe == NULL)
		return ENOMEM;
	ro = malloc(sizeof(struct my_pipe_op), M_TEMP, M_WAITOK);
	if (pipe == NULL)
		goto malloc_fail;
	wo = malloc(sizeof(struct my_pipe_op), M_TEMP, M_WAITOK);
	if (pipe == NULL)
		goto malloc_fail;
	pipe->init = 0;
	pipe->lock = pipe->init + 1;
	pipe->wr_lock = pipe->lock + 1;
	pipe->nreaders = pipe->wr_lock + 1;
	pipe->nwriters = pipe->nreaders + 1;
	pipe->len = pipe->nwriters + 1;
	pipe->in = pipe->len + 4;
	pipe->out = pipe->in + 4;
	pipe->cnt = pipe->out + 4;
	pipe->buf = pipe->cnt + 4;
	pipe->pr_readers = 1;
	pipe->pr_writers = 1;
	/* check if shared memory is initialized and if not then initialize it */
	init = bus_space_read_1(sharme.data_t, sharme.data_h, pipe->init); 
	if (init == 0) {
		smalls[0] = 1;
		smalls[1] = 0;
		smalls[2] = 0;
		smalls[3] = 0;
		smalls[4] = 0;
		write_region_1(pipe->init, smalls, 5);
		bigs[0] = MY_PIPE_BUF_SIZE;
		bigs[1] = 0;
		bigs[2] = 0;
		bigs[3] = 0;
		write_region_4(pipe->len, bigs, 4);
	}
	/* increase reaaders and writters in pipe */
	pipe_lock(pipe->lock);
	read_region_1(pipe->nreaders, nparts, 2);
	nparts[0]++;
	nparts[1]++;
	write_region_1(pipe->nreaders, nparts, 2);
	pipe_unlock(pipe->lock);
	sharme.pipeops = &my_pipeops;
	ro->oper = 0;
	wo->oper = 1;
	ro->pipe = wo->pipe = pipe;
	/* allocate read end of pipe */
	error = fd_allocfile(&rf, &descr);
	if (error)
		return error;
	fd[0] = descr;
	/* allocate write end of pipe */
	error = fd_allocfile(&wf, &descr);
	if (error)
		goto my_pipe_error;
	fd[1] = descr;
	/* initialization of read file_t */
	rf->f_flag = FREAD;
	rf->f_type = DTYPE_MISC;
	rf->f_ops = &my_pipeops;
	rf->f_data = ro;
	fd_set_exclose(l, (int)fd[0], (O_CLOEXEC) != 0);
	/* initialization of write file_t */
	wf->f_flag = FWRITE;
	wf->f_type = DTYPE_MISC;
	wf->f_ops = &my_pipeops;
	wf->f_data = wo;
	fd_set_exclose(l, (int)fd[1], (O_CLOEXEC) != 0);

	/* add those files to the process that made the system call */
	fd_affix(curproc, rf, (int)fd[0]);
	fd_affix(curproc, wf, (int)fd[1]);
	/* return the file descriptors */
	if ((error = copyout(fd, SCARG(uap, fildes), sizeof(fd))) != 0)
		return error;

	*retval = 0;
	return 0;
my_pipe_error:
	fd_abort(curproc, rf, (int)fd[0]);
	return error;
malloc_fail:
	free(pipe, M_TEMP);
	free(ro, M_TEMP);
	free(wo, M_TEMP);
	return ENOMEM;
}

/*
 * Spinlock for pipe 
 */
void pipe_lock(bus_size_t lock)
{
	while(__sync_val_compare_and_swap((uint8_t *)sharme.data_b + lock, 0, 1) == 1)
		/* do nothing */; 
	return;
}

/*
 * Release the lock
 */
void pipe_unlock(bus_size_t lock) 
{
	__sync_lock_release((uint8_t *)sharme.data_b + lock);
	return;
}

/*
 * Read a region of bytes in bus (data is 1 byte)
 */
void read_region_1(bus_size_t offset, uint8_t *datap, bus_size_t count)
{
	int i;
	for (i=0; i<count; i++) {
		datap[i] = bus_space_read_1(sharme.data_t, sharme.data_h, 
				offset + i);
	}
	return;
}

/*
 * Read a region of bytes in bus (data is 4 byte)
 */
void read_region_4(bus_size_t offset, uint32_t *datap, bus_size_t count)
{
	int i;
	for (i=0; i<count; i++) {
		datap[i] = bus_space_read_4(sharme.data_t, sharme.data_h, 
				offset + i*4);
	}
	return;
}

/*
 * Write in a region of bytes in bus (data is 1 byte)
 */
void write_region_1(bus_size_t offset, uint8_t *datap, bus_size_t count)
{
	int i;
	for (i=0; i<count; i++) {
		bus_space_write_1(sharme.data_t, sharme.data_h, offset + i, 
				datap[i]);
	}
	return;
}

/*
 * Write in a region of bytes in bus (data is 4 byte)
 */
void write_region_4(bus_size_t offset, uint32_t *datap, bus_size_t count)
{
	int i;
	for (i=0; i<count; i++) {
		bus_space_write_4(sharme.data_t, sharme.data_h,	offset + i*4, 
				datap[i]);
	}
	return;
}

