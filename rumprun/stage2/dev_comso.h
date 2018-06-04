/*
 *
 * Definitions for the comso
 *
 */
#ifndef COSMO_H
#define COSMO_H 1
#define COSMO_PORT	23456	//the port that will be used

#include <netinet/in.h>

//#include <net/if.h>
//#include <net/netisr.h>
//#include <netinet/in.h>
//#include <netinet/in_var.h>
//#include <netinet/ip_var.h>
//#include <netinet/if_inarp.h>


struct comso_stuff
{
	struct socket *so;
	struct sockaddr_in sa;
};

#endif

