#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
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

//read from a file descriptor 30 bytes
int read_from_descr(int dscr, char *buffer)
{
	int n;
	n = read(dscr, buffer, 30);
	if(n <= 0) {
		if (n < 0)
			perror("read from remote peer failed");
		else
			fprintf(stderr, "Peer went away\n");
		return 0;
	}
	return n;
}

int main()
{
	int sd, newsd; 
	struct sockaddr_in sa;
	char buf[30];
	socklen_t len;
	/* Create TCP/IP socket*/
	sd = create_socket(PF_INET, SOCK_STREAM, 0);
	fprintf(stderr, "Created TCP socket\n");
	/* Bind to a well-known port */
	init_socket_address(&sa, AF_INET, PORT, INADDR_ANY);
	bind_socket(sd, &sa, sizeof(struct sockaddr_in));
	fprintf(stderr, "Bound TCP socket to port %d\n", PORT);
	/* Listen for incoming connections */
	listen_to_socket(sd, 2);
	fprintf(stderr, "Waiting for an incoming connection...\n");
	/* Accept an incoming connection */
	len = sizeof(struct sockaddr_in);
	newsd = wait_new_con(sd, &sa, &len);
	fprintf(stderr, "Incoming connection\n");
	read_from_descr(newsd, buf);
	printf("Got message: %s", buf);
	if (close(newsd) < 0)
		perror("close newsd");
	if (close(sd) < 0)
		perror("close sd");
	printf("Time to die\n");
	return 0;
}

