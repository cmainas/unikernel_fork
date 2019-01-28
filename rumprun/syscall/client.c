#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <sys/ioctl.h>
#include <sys/ioccom.h>

#define SETIPADDR _IOW('f', 132, int *)
#define SEND_IP         (((((192 << 8) | 168) << 8) | 1) << 8) | 11

int main()
{
	int fd[2], n;
	char *buf = "Hello from another unikernel!\n";
	if(my_pipe(fd) < 0)
		perror("pipe");
	printf("USERSPACE: fd: %d - %d\n", fd[0], fd[1]);
	//int ip[5] = {192, 168, 1, 2};
	uint32_t ip = htonl(SEND_IP);
	//printf("Give ip %d:%d:%d:%d\n", ip[0], ip[1], ip[2], ip[3]);
	printf("Give ip %u\n", ip);
	ioctl(fd[1], SETIPADDR, &ip);
	close(fd[0]);
	n = write(fd[1], buf, 30);
	if(n < 0)
		perror("write");
	printf("USERSPACE: wrote %d bytes\n", n);
	close(fd[1]);
	n = write(fd[1], buf, 30);
	/* needs to wait a bit before halt */
	sleep(10);
	return 0;
}


