#include <sys/syscallargs.h>
#include <sys/filedesc.h>
#include <sys/file.h>
#include <sys/proc.h>
#include <sys/socket.h>
#include <sys/socketvar.h>
#include <sys/malloc.h>
#include <netinet/in.h>

static int my_pipe_close(file_t *fp);
static int my_pipe_close(file_t *fp);
static int my_pipe_read(file_t *fp, off_t *offset, struct uio *uio, 
		kauth_cred_t cred, int flags);
static int my_pipe_write(file_t *fp, off_t *offset, struct uio *uio, 
		kauth_cred_t cred, int flags);
static int my_pipe_ioctl(file_t *fp, u_long cmd, void *data);
static int my_bind(struct socket *so, struct lwp *l);

#define SETIPADDR _IOW('f', 132, int *)

const struct fileops my_pipeops = {
	.fo_read = my_pipe_read,
	.fo_write = my_pipe_write,
	.fo_ioctl = my_pipe_ioctl,
	.fo_close = my_pipe_close,
};

struct my_pipe_data {
	struct socket *so;
	uint32_t ip;
	int fd;
};

#define PORT		23456	//the port that will be used
//IP of the server
#define SERVER_IP		(((((192 << 8) | 168) << 8) | 1) << 8) | 13
#define SEND_IP		(((((192 << 8) | 168) << 8) | 1) << 8) | 13


/*
 * Handle the close request 
 */
static int my_pipe_close(file_t *fp)
{
	/* close the socket */
	/* TODO free any allocated memory */
	struct my_pipe_data *mpd = fp->f_data; 
	fp->f_data = NULL;
	soclose(mpd->so);
	return 0; /*this always succeeds */
}

/*
 * Handle the read 
 */
static int my_pipe_read(file_t *fp, off_t *offset, struct uio *uio, 
		kauth_cred_t cred, int flags)
{
	struct my_pipe_data *rmpd = fp->f_data; 
	int ret = 0, rcvflags = 0;;
	struct sockaddr *from = NULL;
	/* Wait for a connection and receive message. 
	** The message is passed to userspace */
	ret = soreceive(rmpd->so, (struct mbuf **) &from, uio, NULL, NULL,
		       	&rcvflags);
	if (from == NULL)
		printf("blakeies\n");
	//struct sockaddr_in *sin = (struct sockaddr_in *) from;
	printf("Incoming connection: %u\n", (unsigned int) from->sa_len);
	return ret;
}

/*
 * Handle the write for the device
 */
static int my_pipe_write(file_t *fp, off_t *offset, struct uio *uio, 
		kauth_cred_t cred, int flags)
{
	struct my_pipe_data *wmpd = fp->f_data; 
	int ret = 0;
	/* Send the message from userspace */
	//printf("KERNEL: send to ip %u\n", wmpd->sin->sin_addr.s_addr);
	struct sockaddr_in *sin, ssin;
	/* prepare the struct sockaddr_in */
	sin = &ssin;
        bzero(sin, sizeof *sin);
        sin->sin_len = sizeof(*sin);
        sin->sin_family = AF_INET;
        //sin->sin_addr.s_addr = htonl(SERVER_IP);
        sin->sin_addr.s_addr = wmpd->ip;
	printf("KERNEL: ip = %u\n", sin->sin_addr.s_addr);
	sin->sin_port = htons(PORT);
	ret = sosend(wmpd->so, (struct sockaddr *)sin, uio, NULL, NULL,
		       	0, curlwp);
	return ret;
}

/*
 * Handle ioctl for my_pipe 
 */
static int my_pipe_ioctl(file_t *fp, u_long cmd, void *data) 
{
	struct my_pipe_data *wmpd = fp->f_data; 
	int ret = 0;
	//int *ip = data;
	uint32_t *ip = data;
	switch (cmd) {
		case SETIPADDR:
			//printf("KERNEL: Got ip %d:%d:%d:%d\n", ip[0], ip[1], ip[2], ip[3]);
			printf("KERNEL: Got ip %u\n", *ip);
			printf("KERNEL: other ip %u\n", htonl(SEND_IP));
			wmpd->ip = *ip;
			break;
		default: 
			ret = ENODEV;
	}
	return ret;
}

static int my_bind(struct socket *so, struct lwp *l)
{
	int ret = 0;
	struct sockaddr_in *sin, ssin;
	sin = &ssin;
        bzero(sin, sizeof(struct sockaddr_in));
        sin->sin_len = sizeof(struct sockaddr_in);
        sin->sin_family = AF_INET;
        sin->sin_addr.s_addr = INADDR_ANY;
	sin->sin_port = htons(PORT);
	/* Bind to a well-known port */
	if ((ret = sobind(so, (struct sockaddr *)sin, l)) != 0) 
		return ret;
	printf("Bound UDP socket to port %d\n", PORT);
	return ret;
}

int sys_my_pipe(struct lwp *l, const struct sys_my_pipe_args *uap, 
		register_t *retval)
{
	file_t *rf, *wf;
	int fd[2], error, descr;
	struct my_pipe_data *rd, *wd;
	struct socket *rso, *wso;
	//struct sockaddr_in *rsin, *wsin;
	/* allocate memory */
	if ((rd = malloc(sizeof(struct my_pipe_data), M_TEMP, M_WAITOK)) == 
		NULL)
		return ENOMEM;
	if ((wd = malloc(sizeof(struct my_pipe_data), M_TEMP, M_WAITOK)) == 
		NULL)
		return ENOMEM;
	/* create sockets */
	if ((error = socreate(AF_INET, &(rso), SOCK_DGRAM, 0, l, NULL)) 
			!= 0)
		return error;
	if ((error = socreate(AF_INET, &(wso), SOCK_DGRAM, 0, l, NULL)) 
			!= 0)
		return error;
	printf("Created UDP sockets\n");
	if ((error = my_bind(rso, l)) != 0)
		return error;
	rd->so = rso;
	rd->ip = 0;
	wd->so = wso;
	wd->ip = 0;

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
