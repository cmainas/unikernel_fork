#include <sys/syscallargs.h>
#include "my_pipe.h"
#include <sys/filedesc.h>
#include <sys/file.h>
#include <sys/proc.h>
#include <sys/bus.h>

#define VMCALL_ID 100

/* hypercall using io vm exit */
static inline uint32_t inl(uint16_t port)
{
	uint32_t rv;
	__asm__ __volatile__("inl %1, %0" : "=a"(rv) : "d"(port));
	return rv;
}

//static void outb(uint8_t value, uint16_t port) 
//{
//        asm("outb %0,%1" : /* empty */ : "a" (value), "Nd" (port) : "memory");
//}

static void increase_pipe_rw(bus_size_t n, bus_size_t lock)
{
	uint8_t a;
	pipe_lock(lock);
	a = bus_space_read_1(sharme.data_t, sharme.data_h, n);
	a++;
	printf(" increase: %d\n", a);
	bus_space_write_1(sharme.data_t, sharme.data_h, n, a);
	pipe_unlock(lock);
}

int sys_my_fork(struct lwp *l, const void *v, register_t *retval)
{
	//outb(77, 0xffdc);
	/* check for opened pipes */
	fdfile_t *ff;
	file_t *fp;
	fdtab_t *dt;
	size_t fd;
	filedesc_t *fdp = l->l_fd;
	dt = fdp->fd_dt;
	int flag = 0;
	for (fd = 0; fd < dt->dt_nfiles; fd++) {
		if ((ff = dt->dt_ff[fd]) == NULL)
			continue;
		if ((fp = ff->ff_file) == NULL)
			continue;
		if (fp->f_ops == sharme.pipeops) {
			/* use of ivshmem */
			flag = 1;
			struct my_pipe_op *pipe_op = fp->f_data;
			if (pipe_op->oper == 0)
				/* increase readers by one */
				increase_pipe_rw(pipe_op->pipe->nreaders, 
						pipe_op->pipe->lock);
			else if (pipe_op->oper == 1)
				/* increase writers by one */
				increase_pipe_rw(pipe_op->pipe->nwriters, 
						pipe_op->pipe->lock);
		}
	}
	//file_t *file;
	//filedesc_t *fd_t = l->l_fd;
	//struct my_pipe_op *pipe_op;
	//int i = 0;
	//u_int files_num = fd_t->fd_dt->dt_nfiles;
	//for (i = 0; i < files_num; i++) {
	//	file = fd_getfile(fd_t, i);
	//	if (file->f_type == DTYPE_MISC) {
	//		pipe_op = file->f_data;
	//		if (pipe_op->oper == 0)
	//			/* increase readers by one */
	//			increase_pipe_rw(pipe_op->pipe->nreaders, 
	//					pipe_op->pipe->lock);
	//		else if (pipe_op->oper == 1)
	//			/* increase writers by one */
	//			increase_pipe_rw(pipe_op->pipe->nwriters, 
	//					pipe_op->pipe->lock);
	//	}
	//}

	/* start migration */
	unsigned int ret; 
	ret = inl(0xffdd);
	/* wait until migration is over */
	ret = inl(0xffdc);
	while (ret == 0) {
		ret = inl(0xffdc);
	}
	/* when migration is finished child will get 2, 
	 * while parent will get 1
	 */
	if (ret == 1) {
		/* parent return process id of new qeemu instance */
		*retval = inl(0xffdc);
		if (flag == 1) {
			while( bus_space_read_1(sharme.data_t, sharme.data_h, 
						sharme.data_s - 1) != 77)
				/* wait for the child to start */;
		}
	} else  {
		/* child return 0 */
		*retval = 0;
		if (flag == 1) {
			bus_space_write_1(sharme.data_t, sharme.data_h, 
					sharme.data_s - 1, 77);
		}
	}
	return 0;
}
