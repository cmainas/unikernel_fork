#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

void server(int fd[2])
{
	int n;
	char buf[30], buf1[30];
	close(fd[1]);
	printf("wait toread...\n");
	n = read(fd[0], buf, 30);
	if (n < 0)
		perror("read:");
	printf("USERSPACE: read %d bytes\n", n);
	printf("USERSPACE: Got message: %s\n", buf);
	n = read(fd[0], buf, 30);
	if(n < 0)
		perror("read");
	printf("USERSPACE: read %d bytes\n", n);
	printf("USERSPACE: Got message: %s\n", buf);
	printf("wait toread...\n");
	n = read(fd[0], buf1, 30);
	if (n < 0)
		perror("read:");
	printf("USERSPACE: read %d bytes\n", n);
	printf("USERSPACE: Got message: %s\n", buf1);
	close(fd[0]);
	return;
}

void client(int fd[2])
{
	int n;
	char *buf = "Hello from another unikernel!\n";
	n = write(fd[1], buf, 30);
	if(n < 0)
		perror("write");
	printf("USERSPACE: wrote %d bytes\n", n);
	fflush(stdout);
	buf[0] = 'S';
	n = write(fd[1], buf, 30);
	if (n < 0)
		perror("write:");
	printf("USERSPACE: wrote %d bytes\n", n);
	buf[0] = 'Q';
	n = write(fd[1], buf, 20);
	if(n < 0)
		perror("write");
	printf("USERSPACE: wrote %d bytes\n", n);
	close(fd[1]);
	return;
}

int main()
{
	int fd[2], n;
	if(my_pipe(fd) < 0)
		perror("pipe");
	printf("USERSPACE: fd: %d - %d\n", fd[0], fd[1]);
	n = my_fork();
	printf("USERSPACE: fork returned: %d\n", n);
	if (n == 0)
		/* child */
		client(fd);
	else if (n < 0) {
		perror("fork");
		exit(1);
	}
	else 
		server(fd);
	return 0;
	//close(fd[0]);
	//n = write(fd[1], buf, 30);
	//if(n < 0)
	//	perror("write");
	//printf("USERSPACE: wrote %d bytes\n", n);
	//fflush(stdout);
	//buf[0] = 'S';
	//n = write(fd[1], buf, 30);
	//if (n < 0)
	//	perror("write:");
	//printf("USERSPACE: wrote %d bytes\n", n);
	//buf[0] = 'Q';
	//n = write(fd[1], buf, 20);
	//if(n < 0)
	//	perror("write");
	//printf("USERSPACE: wrote %d bytes\n", n);
	//close(fd[1]);
	//return 0;
	//int fd[2], n;
	//char *buf = "Hello from another unikernel!\n";
	//if(my_pipe(fd) < 0)
	//	perror("pipe");
	//printf("USERSPACE: fd: %d - %d\n", fd[0], fd[1]);
	//fflush(stdout);
	//n = my_fork();
	//printf("USERSPACE: fork returned: %d\n", n);
	//n = write(fd[1], buf, 30);
	//if(n < 0)
	//	perror("write");
	//printf("USERSPACE: wrote %d bytes\n", n);
	//fflush(stdout);
	//printf("wait toread...\n");
	//n = read(fd[0], buf, 30);
	//if (n < 0)
	//	perror("read:");
	//printf("USERSPACE: read %d bytes\n", n);
	//printf("USERSPACE: Got message: %s\n", buf);
	//fflush(stdout);
	//sleep(5);
	//buf[0] = 'Q';
	//n = write(fd[1], buf, 20);
	//if(n < 0)
	//	perror("write");
	//printf("USERSPACE: wrote %d bytes\n", n);
	//fflush(stdout);
	//close(fd[0]);
	//close(fd[1]);
	//return 0;
}


