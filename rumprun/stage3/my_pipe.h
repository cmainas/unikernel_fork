#include <sys/bus.h>

struct my_pipe_data {
	int fd;
	bus_size_t		data_s;
	bus_addr_t		data_b;
};

struct my_pipe_generic {
	bus_size_t		data_s;
	bus_addr_t		data_b;
} mp_gen;

