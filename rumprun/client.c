#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <netdb.h>
#include <arpa/inet.h>

#define PORT	23456	//The port that will be used

//create a socket and check for any errors
int create_socket(int dom, int type, int proto)
{
	int descr = socket(dom, type, proto);
	if(descr < 0) {
		perror("socket");
		exit(1);
	}
	return descr;
}

//connect to remote socket and check for errors
void connect_wrapper(int sock, struct sockaddr_in *saddr, socklen_t len)
{
	if (connect(sock, (struct sockaddr *) saddr, len) < 0) {
		perror("connect");
		exit(1);
	}
	return;
}

// Insist until all of the data has been written
ssize_t insist_write(int fd, const void *buf, size_t cnt)
{
	ssize_t ret;
	size_t orig_cnt = cnt;
	
	while (cnt > 0) {
	        ret = write(fd, buf, cnt);
	        if (ret < 0)
	                return ret;
	        buf += ret;
	        cnt -= ret;
	}

	return orig_cnt;
}

//get the ip of hostname and check for errors
struct hostent *get_host_addr(char *h) {
	struct hostent *hp;
	hp = gethostbyname(h);
	if (!hp) {
		printf("DNS lookup failed for host %s\n", h);
		exit(1);
	}
	return hp;
}

int main(int argc, char *argv[])
{
	int sd; 
	struct sockaddr_in sa;
	char *buf = "Hello from another unikernel!\n";
	struct hostent *hp;
	//check command line argunments
	if(argc < 2) {
		fprintf(stderr, "Usage: %s [IP address or hostname of server]\n"
				, argv[0]);
		exit(1);
	}
	// Create TCP/IP socket
	sd = create_socket(PF_INET, SOCK_STREAM, 0);
	fprintf(stderr, "Created TCP socket\n");
	// Bind to a well-known port 
	hp = get_host_addr(argv[1]);
	fprintf(stderr, "Connecting to remote host... ");
	//prepare the struct sockaddr_in for the connect system call
	sa.sin_family = AF_INET;
	sa.sin_port = htons(PORT);
	memcpy(&sa.sin_addr.s_addr, hp->h_addr, sizeof(struct in_addr));
	connect_wrapper(sd, &sa, sizeof(sa));
	fprintf(stderr, "Connected.\n");
	fprintf(stderr, "Send message\n");
	if(insist_write(sd, buf, 30) != 30) {
		perror("write");
		exit(1);
	}
	if (close(sd) < 0)
		perror("close sd");
	printf("Time to die\n");
	return 0;
}

