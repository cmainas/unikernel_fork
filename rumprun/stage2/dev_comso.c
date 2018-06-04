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

//#include <net/if.h>
//#include <net/netisr.h>
//#include <netinet/in.h>
//#include <netinet/in_var.h>
//#include <netinet/ip_var.h>
//#include <netinet/if_inarp.h>

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

#define PORT	23456	//The port that will be used
//The port that will be used
#define SERVER_IP	(((((192 << 8) | 168) << 8) | 1) << 8) | 7

struct comso_stuff cs;

//uint32_t htonl (uint32_t x)
//{
//#if BYTE_ORDER == BIG_ENDIAN
//  return x;
//#elif BYTE_ORDER == LITTLE_ENDIAN
//  return __bswap_32 (x);
//#else
//# error "What kind of system is this?"
//#endif
//}
//
//uint16_t htons (uint16_t x)
//{
//#if BYTE_ORDER == BIG_ENDIAN
//  return x;
//#elif BYTE_ORDER == LITTLE_ENDIAN
//  return __bswap_16 (x);
//#else
//# error "What kind of system is this?"
//#endif
//}

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
	//struct comso_stuff *cs;
	int ret = 0;
	//cs = malloc(struct comso_stuff, M_TEMP, M_WAITOK);
	//if(!cs) {
	//	//No memory
	//	ret = ENOMEM;
	//	goto comso_open_err_ret;
	//}
	// Create TCP/IP socket
	if ((ret = socreate(AF_INET, &(cs.so), SOCK_DGRAM, 0, process, NULL)) 
			!= 0)
		goto comso_open_err_ret;
	printf("Created UDP socket\n");
	//process->l_private = cs;
comso_open_err_ret:
	//free(cs, M_TEMP);
	//process->l_private = NULL;
	return ret;
}

/*
 * Handle the close request for the device.
 */
int comsoclose(dev_t device, int flags, int fmt, struct lwp *process)
{
	printf("Hello from comsoclose\n");
	//struct comso_stuff *cs = process->l_private;
	soclose(cs.so);
	//free(cs, M_TEMP);
	return 0; /* again this always succeeds */
}

/*
 * Handle the read for the device
 */
int comsoread(dev_t device, struct uio *uio, int flags)
{
	//char *buffer;
	int ret = 0;
	struct lwp *l = curlwp;
	struct sockaddr_in *sin, ssin;
	//struct socket *new_so;
	// Bind to a well-known port 
	printf("Hello from comsoread\n");
	sin = &ssin;
        bzero(sin, sizeof *sin);
        sin->sin_len = sizeof(*sin);
        sin->sin_family = AF_INET;
        sin->sin_addr.s_addr = INADDR_ANY;
	sin->sin_port = htons(PORT);
	if ((ret = sobind(cs.so, (struct sockaddr *)sin, l)) != 0) {
		printf("Ret: %d\n", ret);
		goto comso_read_ret;
	}
	printf("Bound TCP socket to port %d\n", PORT);
	//Listen for incoming connections 
	//if ((ret = solisten(cs.so, 2, l)) != 0)
	//	goto comso_read_ret;
	printf("Creating a new connection\n");
	//new_so = sonewconn(cs.so, true);
	//new_so->so_state &= ~SS_NBIO;
	printf("Waiting for a new connection...\n");
	//if ((ret = soaccept(new_so, (struct sockaddr *)sin)) != 0) 
	//	goto comso_read_ret;
	//struct mbuf *mp = NULL;
	struct sockaddr *from = NULL;
	int rcvflags = 0;
	//ret = soreceive(cs.so, (struct mbuf **) &from, uio, &mp	
	ret = soreceive(cs.so, (struct mbuf **) &from, uio, NULL
			,NULL, &rcvflags);
		//goto comso_read_ret;
	printf("Incoming connection\n");
	printf("got %ld bytes\n", uio->uio_resid);
	//if (n < 0) {
	//	ret = -1;
	//	goto comso_read_ret;
	//}
	//buffer = mtod(mp, char *);
	//printf("Got message: %s", buffer);
	//uiomove(buffer, 30, uio);
comso_read_ret:
	return ret;
}

/*
 * Handle the write for the device
 */
int comsowrite(dev_t device, struct uio *uio, int flags)
{
	//printf("Hello from comsowrite\n");
	struct lwp *l = curlwp;
	int ret = 0;
	struct sockaddr_in *sin, ssin;
	//struct comso_stuff *cs = process->l_private;
	//struct hostent *hp;
	//hp = get_host_addr(SERVER_IP);
	printf("Connecting to remote host... ");
	//prepare the struct sockaddr_in for the connect system call
	sin = &ssin;
        bzero(sin, sizeof *sin);
        sin->sin_len = sizeof(*sin);
        sin->sin_family = AF_INET;
        sin->sin_addr.s_addr = htonl(SERVER_IP);
	sin->sin_port = htons(PORT);
	//sa.sin_family = AF_INET;
	//sa.sin_port = htons(PORT);
	//sa.sin_addr.s_addr = htonl(SERVER_IP);
	printf("(%ld) ", (long)ssin.sin_addr.s_addr);
	//memcpy(&cs.sa.sin_addr.s_addr, hp->h_addr, sizeof(struct in_addr));
	//if ((ret = soconnect(cs.so, (struct sockaddr *) &(cs.sa), l)) != 0) 
	//	goto comso_write_ret;
	//if ((ret = sobind(cs.so, (struct sockaddr *)sin, l)) != 0) {
	//	printf("Ret: %d\n", ret);
	//	goto comso_write_ret;
	//}
	printf("Connected.\n");
	//char *buffer;
       	//int len = uio->uio_iov->iov_len;
	//buffer = malloc(len, M_TEMP, M_WAITOK);
	//uiomove(buffer, len, uio);
	//printf("Send message: %s\n", buffer);
	//struct mbuf *mp = m_gethdr(M_WAIT, MT_DATA);
	//while (cnt > 0) {
	//        ret = write(cs.socket, buffer, cnt);
	//        if (ret < 0)
	//                goto comso_write_ret;
	//        buffer += ret;
	//        cnt -= ret;
	//}
	//ret = len;
	printf("To send %ld bytes\n", uio->uio_resid);
	ret = sosend(cs.so, (struct sockaddr *)sin, uio, NULL, NULL, 0, l);
	printf("Sent %d bytes\n", ret);
//comso_write_ret:
	return ret;
}
