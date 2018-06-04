#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>

int main()
{
	int d = open("/dev/comso", O_WRONLY), n;
	char *buf = "Hello from another unikernel!\n";
	printf("CLIENT: fd: %d\n",d);
	n = write(d, buf, 30);
	if(n < 0)
		perror("write");
	printf("CLIENT: wrote %d bytes\n", n);
	close(d);
	sleep(5);
	return 0;
}


