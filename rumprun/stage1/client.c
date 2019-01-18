#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <netdb.h>
#include <arpa/inet.h>

#define PORT	23456	//The port that will be used
struct hostent *hp;

//bind the socket and check for any errors
void bind_socket(int sock, struct sockaddr_in *addr, socklen_t len)
{
	if (bind(sock, (struct sockaddr *)addr, len) < 0) {
		perror("bind");
		exit(1);
	}
	return;
}

//start listening to a socket and check for errors
void listen_to_socket(int sock, int bcklog)
{
	if (listen(sock, bcklog) < 0) {
		perror("listen");
		exit(1);
	}
	return;
}

//accept a new connection and check for errors
int wait_new_con(int sock, struct sockaddr_in *saddr, socklen_t *addr_len)
{
	int new;
	new = accept(sock, (struct sockaddr *)saddr, addr_len);
	if (new < 0) {
		perror("accept");
		exit(1);
	}
	return new;
}

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

//prepare the struct sockaddr_in for the bind system call
void init_socket_address(struct sockaddr_in *saddr, sa_family_t fam,
	       	uint16_t port, uint32_t addr)
{
	memset(saddr, 0, sizeof(struct sockaddr_in));
	saddr->sin_family = fam;
	saddr->sin_port = htons(port);
	saddr->sin_addr.s_addr = htonl(addr);
	return;
}

int my_pipe(int fildes[])
{
	socklen_t len;
	int rsd, wsd, newrsd;
	struct sockaddr_in sa;
	// Write part of pipe
	// Create TCP/IP socket
	wsd = create_socket(PF_INET, SOCK_STREAM, 0);
	fprintf(stderr, "Created TCP socket\n");
	// Bind to a well-known port 
	fprintf(stderr, "Connecting to remote host... ");
	//prepare the struct sockaddr_in for the connect system call
	sa.sin_family = AF_INET;
	sa.sin_port = htons(PORT);
	memcpy(&sa.sin_addr.s_addr, hp->h_addr, sizeof(struct in_addr));
	//init_socket_address(&sa, AF_INET, PORT, hp->h_addr);
	connect_wrapper(wsd, &sa, sizeof(sa));
	fprintf(stderr, "Connected.\n");
	fildes[1] = wsd;
	// READ part of pipe
	/* Create TCP/IP socket*/
	rsd = create_socket(PF_INET, SOCK_STREAM, 0);
	fprintf(stderr, "Created TCP socket\n");
	/* Bind to a well-known port */
	init_socket_address(&sa, AF_INET, PORT, INADDR_ANY);
	bind_socket(rsd, &sa, sizeof(struct sockaddr_in));
	fprintf(stderr, "Bound TCP socket to port %d\n", PORT);
	/* Listen for incoming connections */
	listen_to_socket(rsd, 2);
	fprintf(stderr, "Waiting for an incoming connection...\n");
	/* Accept an incoming connection */
	len = sizeof(struct sockaddr_in);
	newrsd = wait_new_con(rsd, &sa, &len);
	fprintf(stderr, "Incoming connection\n");
	fildes[0] = newrsd;
	return 0;
}

int main(int argc, char *argv[])
{
	int pipe[2]; 
	char *buf = "Hello from another unikernel!\n";
	//check command line argunments
	if(argc < 2) {
		fprintf(stderr, "Usage: %s [IP address or hostname of server]\n"
				, argv[0]);
		exit(1);
	}
	hp = get_host_addr(argv[1]);
	if (my_pipe(pipe) != 0) {
		fprintf(stderr, "eroor creating pipe\n");
		return 1;
	}
	fprintf(stderr, "Send message\n");
	if(insist_write(pipe[1], buf, 30) != 30) {
		perror("write");
		exit(1);
	}
	if (close(pipe[0]) < 0)
		perror("close read end");
	if (close(pipe[1]) < 0)
		perror("close write end");
	printf("Time to die\n");
	return 0;
}

