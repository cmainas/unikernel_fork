#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>

int main()
{
	int fd[2], n;
	char buf[30];
	if(my_pipe(fd) < 0)
		perror("pipe");
	printf("USERSPACE: fd: %d - %d\n", fd[0], fd[1]);
	//sleep(10);
	printf("wait toread...\n");
	for (int i = 0; i < 66; i++){
		n = read(fd[0], buf, 30);
		if (n < 0)
			perror("read:");
		printf("USERSPACE: read %d bytes\n", n);
		printf("USERSPACE: Got message: %s\n", buf);
	}
	//buf[0] = 'S';
	//sleep(5);
	//n = write(fd[1], buf, 30);
	//if(n < 0)
	//	perror("write");
	//printf("USERSPACE: wrote %d bytes\n", n);
	//printf("wait toread...\n");
	//n = read(fd[0], buf, 30);
	//if (n < 0)
	//	perror("read:");
	//printf("USERSPACE: read %d bytes\n", n);
	//printf("USERSPACE: Got message: %s\n", buf);
	close(fd[0]);
	close(fd[1]);
	return 0;
}

