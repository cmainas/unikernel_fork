#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>

int main()
{
	int d = open("/dev/comso", O_WRONLY), n;
	char *buf = "Hello from another unikernel!\n";
	printf("USERSPACE: fd: %d\n",d);
	n = write(d, buf, 30);
	if(n < 0)
		perror("write");
	printf("USERSPACE: wrote %d bytes\n", n);
	close(d);
	/* needs to wait a bit before halt */
	sleep(1);
	return 0;
}


