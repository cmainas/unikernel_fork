/*-
 * A simple character device driver that implements a socket communication
 *
 */

#include <sys/proc.h>
#include <sys/conf.h>

#include "dev_comso.h"

#include <sys/types.h>
#include <sys/socket.h>
#include <sys/socketvar.h>
#include <sys/filedesc.h>
#include <netinet/in.h>
#include <sys/malloc.h>

dev_type_open(comsoopen);
dev_type_close(comsoclose);
dev_type_write(comsowrite);
dev_type_read(comsoread);

/* Autoconfiguration glue */
void comsoattach(int num);
int comsoopen(dev_t device, int flags, int fmt, struct lwp *process);
int comsoread(dev_t device, struct uio *uio, int flags);
int comsowrite(dev_t device, struct uio *uio, int flags);
int comsoclose(dev_t device, int flags, int fmt, struct lwp *process);

/* just define the character device handlers because that is all we need */
const struct cdevsw comso_cdevsw = {
        .d_open = comsoopen,
	.d_close = comsoclose,
	.d_read = comsoread,
	.d_write = comsowrite,

        .d_ioctl = noioctl,
	.d_stop = nostop,
	.d_tty = notty,
	.d_poll = nopoll,
	.d_mmap = nommap,
	.d_kqfilter = nokqfilter,
	.d_discard = nodiscard,
	.d_flag = D_OTHER
};


struct socket *so;

/*
 * Attach for autoconfig to find.  The parameter is the number given in
 * the configuration file, and defaults to 1.  New code should not expect
 * static configuration from the kernel config file, and thus ignore that
 * parameter.
 */
void comsoattach(int num)
{
	printf("Hello from comsoattach\n");
	return;
	  /* nothing to do for comso, this is where resources that
	     need to be allocated/initialised before open is called
	     can be set up */
}

/*
 * Handle an open request on the device.
 */
int comsoopen(dev_t device, int flags, int fmt, struct lwp *process)
{
	int ret = 0;
	// Create UDP socket
	if ((ret = socreate(AF_INET, &(so), SOCK_DGRAM, 0, process, NULL)) 
			!= 0)
		return ret;
	printf("Created UDP socket\n");
	return ret;
}

/*
 * Handle the close request for the device.
 */
int comsoclose(dev_t device, int flags, int fmt, struct lwp *process)
{
	/* close the socket */
	soclose(so);
	return 0; /*this always succeeds */
}

/*
 * Handle the read for the device
 */
int comsoread(dev_t device, struct uio *uio, int flags)
{
	int ret = 0, rcvflags = 0;;
	struct lwp *l = curlwp;
	struct sockaddr_in *sin, ssin;
	struct sockaddr *from = NULL;
	/* Prepare struct sockaddr_in for bind */
	sin = &ssin;
        bzero(sin, sizeof *sin);
        sin->sin_len = sizeof(*sin);
        sin->sin_family = AF_INET;
        sin->sin_addr.s_addr = INADDR_ANY;
	sin->sin_port = htons(PORT);
	/* Bind to a well-known port */
	if ((ret = sobind(so, (struct sockaddr *)sin, l)) != 0) {
		printf("Ret: %d\n", ret);
		goto comso_read_ret;
	}
	printf("Bound UDP socket to port %d\n", PORT);
	printf("Waiting for a new connection...\n");
	/* Wait for a connection and receive message. 
	** The message is passed to userspace */
	ret = soreceive(so, (struct mbuf **) &from, uio, NULL, NULL, &rcvflags);
	printf("Incoming connection\n");
comso_read_ret:
	return ret;
}

/*
 * Handle the write for the device
 */
int comsowrite(dev_t device, struct uio *uio, int flags)
{
	struct lwp *l = curlwp;
	int ret = 0;
	struct sockaddr_in *sin, ssin;
	printf("Connecting to remote host... \n");
	/* prepare the struct sockaddr_in */
	sin = &ssin;
        bzero(sin, sizeof *sin);
        sin->sin_len = sizeof(*sin);
        sin->sin_family = AF_INET;
        sin->sin_addr.s_addr = htonl(SERVER_IP);
	sin->sin_port = htons(PORT);
	/* Send the message from userspace */
	ret = sosend(so, (struct sockaddr *)sin, uio, NULL, NULL, 0, l);
	return ret;
}
