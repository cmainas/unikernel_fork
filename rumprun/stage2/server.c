#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>

int main()
{
	int d = open("/dev/comso", O_RDONLY), n;
	char buf[30];
	printf("USERSPACE: fd: %d\n",d);
	n = read(d, buf, 30);
	if (n < 0)
		perror("read:");
	printf("USERSPACE: read %d bytes\n", n);
	printf("USERSPACE: Got message: %s\n", buf);
	close(d);
	return 0;
}

