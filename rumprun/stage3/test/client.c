#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>

int main()
{
	int fd[2], n;
	char *buf = "Hello from another unikernel!\n";
	if(my_pipe(fd) < 0)
		perror("pipe");
	printf("USERSPACE: fd: %d - %d\n", fd[0], fd[1]);
	fflush(stdout);
	n = write(fd[1], buf, 30);
	if(n < 0)
		perror("write");
	printf("USERSPACE: wrote %d bytes\n", n);
	fflush(stdout);
	printf("wait toread...\n");
	n = read(fd[0], buf, 30);
	if (n < 0)
		perror("read:");
	printf("USERSPACE: read %d bytes\n", n);
	printf("USERSPACE: Got message: %s\n", buf);
	fflush(stdout);
	sleep(5);
	buf[0] = 'Q';
	n = write(fd[1], buf, 30);
	if(n < 0)
		perror("write");
	printf("USERSPACE: wrote %d bytes\n", n);
	fflush(stdout);
	close(fd[0]);
	close(fd[1]);
	/* needs to wait a bit before halt */
	return 0;
}


