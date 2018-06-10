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


