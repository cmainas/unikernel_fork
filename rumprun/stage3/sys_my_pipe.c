#include <sys/syscallargs.h>
#include <sys/filedesc.h>
#include <sys/file.h>
#include <sys/proc.h>
#include <sys/socket.h>
#include <sys/socketvar.h>
#include <sys/malloc.h>
#include <netinet/in.h>

#include "my_pipe.h"

int my_pipe_close(file_t *fp);
int my_pipe_read(file_t *fp, off_t *offset, struct uio *uio, kauth_cred_t cred,
		int flags);
int my_pipe_write(file_t *fp, off_t *offset, struct uio *uio, kauth_cred_t cred,
		int flags);
int my_bind(struct sockaddr_in *sin, struct socket *so, struct lwp *l);
void prep_send(struct sockaddr_in *sin);

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
	/* close the socket */
	/* TODO free any allocated memory */
	//struct my_pipe_data *mpd = fp->f_data; 
	fp->f_data = NULL;
	return 0; /*this always succeeds */
}

/*
 * Handle the read 
 */
int my_pipe_read(file_t *fp, off_t *offset, struct uio *uio, kauth_cred_t cred,
		int flags)
{
	struct my_pipe_data *rmpd = fp->f_data; 
	int ret = 0;
	while (uio->uio_resid) {
		ret = uiomove((char *)rmpd->data_b, uio->uio_resid, uio);
		if(ret) {
			break;
		}
	}
	return ret;
}

/*
 * Handle the write for the device
 */
int my_pipe_write(file_t *fp, off_t *offset, struct uio *uio, kauth_cred_t cred,
	       	int flags)
{
	struct my_pipe_data *wmpd = fp->f_data; 
	int ret = 0;
	/* Send the message from userspace */
	while (uio->uio_resid) {
		ret = uiomove((char *)wmpd->data_b, uio->uio_resid, uio);
		if(ret)
			break;
	}
	return ret;
}

int sys_my_pipe(struct lwp *l, const struct sys_my_pipe_args *uap, 
		register_t *retval)
{
	file_t *rf, *wf;
	int fd[2], error, descr;
	struct my_pipe_data *rd, *wd;
	/* allocate my_pipe_data structs and initialize them */
	if ((rd = malloc(sizeof(struct my_pipe_data), M_TEMP, M_WAITOK)) == 
		NULL)
		return ENOMEM;
	if ((wd = malloc(sizeof(struct my_pipe_data), M_TEMP, M_WAITOK)) == 
		NULL)
		return ENOMEM;
	rd->data_s = mp_gen.data_s;
	rd->data_b = mp_gen.data_b;
	wd->data_s = mp_gen.data_s;
	wd->data_b = mp_gen.data_b;
	/* allocate read end of pipe */
	error = fd_allocfile(&rf, &descr);
	if (error)
		return error;
	fd[0] = descr;
	rd->fd = fd[0];
	/* allocate write end of pipe */
	error = fd_allocfile(&wf, &descr);
	if (error)
		goto my_pipe_error;
	fd[1] = descr;
	wd->fd = fd[1];
	/* initialization of read file_t */
	rf->f_flag = FREAD;
	rf->f_type = DTYPE_MISC;
	rf->f_ops = &my_pipeops;
	rf->f_data = rd;
	fd_set_exclose(l, (int)fd[0], (O_CLOEXEC) != 0);
	/* initialization of write file_t */
	wf->f_flag = FWRITE;
	wf->f_type = DTYPE_MISC;
	wf->f_ops = &my_pipeops;
	wf->f_data = wd;
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
}
