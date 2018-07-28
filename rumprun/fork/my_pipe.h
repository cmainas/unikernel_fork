#include <sys/bus.h>

#define	MY_PIPE_BUF_SIZE	1024

struct my_pipe {
	bus_size_t	init;		/* is shared memory initalized? */
	bus_size_t	lock;		/* pipe lock */
	bus_size_t	wr_lock;	/* writers lock */
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
	bus_size_t		data_s;		/* size of shared memory */
	bus_addr_t		data_b;		/* base address of shared
						   memory */
	bus_space_tag_t		data_t;		/* bus tag for shared memory */
	bus_space_handle_t	data_h;		/* bus handle for shared 
						   memory */
	const struct fileops	*pipeops;	/* needed for checking if open 
						   file has type my_pipe */
} sharme;

void pipe_lock(bus_size_t lock);
void pipe_unlock(bus_size_t lock);
void read_region_1(bus_size_t offset, uint8_t *datap, bus_size_t count);
void read_region_4(bus_size_t offset, uint32_t *datap, bus_size_t count);
void write_region_1(bus_size_t offset, uint8_t *datap, bus_size_t count);
void write_region_4(bus_size_t offset, uint32_t *datap, bus_size_t count);

