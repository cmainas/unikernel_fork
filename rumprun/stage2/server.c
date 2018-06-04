#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>

int main()
{
	int d = open("/dev/comso", O_RDONLY), n;
	char buf[30];
	printf("SERVER: fd: %d\n",d);
	n = read(d, buf, 30);
	if (n < 0)
		perror("read:");
	printf("SERVER: read %d bytes\n", n);
	printf("SERVER: Got message: %s\n", buf);
	close(d);
	return 0;
}

