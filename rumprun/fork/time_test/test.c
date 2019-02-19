#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

#include <sys/time.h> 
#define USEC            1000000

int main()
{
	int fd[2], n;
	struct timeval t1, t2;
	gettimeofday(&t1, 0);
	//n = fork();
	n = my_fork();
	if (n == 0) {
		/* child */
		gettimeofday(&t2, 0);
		printf("child: fork took: %lfs\n", (double)((t2.tv_sec - t1.tv_sec) * USEC + t2.tv_usec - t1.tv_usec) / USEC);
	} else if (n < 0) {
		perror("fork");
		exit(1);
	} else {
		gettimeofday(&t2, 0);
		printf("parent: fork took: %lfs\n", (double)((t2.tv_sec - t1.tv_sec) * USEC + t2.tv_usec - t1.tv_usec) / USEC);
	}
	return 0;
}


