#include <sys/bus.h>

#define	MY_PIPE_BUF_SIZE	1024

struct my_pipe {
	bus_size_t	init;		/* is shared memory initalized? */
	bus_size_t	lock;		/* pipe lock */
	bus_size_t	nreaders;	/* number of readers in pipe */
	bus_size_t	nwriters;	/* number of writers in pipe */
	bus_size_t	len;		/* size of pipe buffer */
	bus_size_t	in;		/* pointer for next write */
	bus_size_t	out;		/* pointer for next read */
	bus_size_t	cnt;		/* number of bytes in pipe */
	bus_size_t	buf;		/* pipe buffer */
	int		pr_readers;	/* readers from this process */
	int		pr_writers;	/* writers from curr process */
};

struct my_pipe_op {
	int		oper;		/* operation in pipe 0 for read, 
					   1 for write*/
	struct my_pipe	*pipe;
};

struct ivshm {
	bus_size_t		data_s;
	bus_addr_t		data_b;
	bus_space_tag_t		data_t;
	bus_space_handle_t	data_h;
} sharme;

