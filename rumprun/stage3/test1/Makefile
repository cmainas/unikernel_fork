CC = /path/to/x86_64-rumprun-netbsd-gcc
BK = /path/to/rumprun-bake

CFLAGS = -Wall

TARGET = hw_generic_iv

BINS = server-rumprun.bin clienta-rumprun.bin client-rumprun.bin

all: $(BINS)

server-rumprun.bin: server-rumprun
	$(BK) $(TARGET) server-rumprun.bin server-rumprun

clienta-rumprun.bin: clienta-rumprun
	$(BK) $(TARGET) clienta-rumprun.bin clienta-rumprun

client-rumprun.bin: client-rumprun
	$(BK) $(TARGET) client-rumprun.bin client-rumprun

server-rumprun: server.c
	$(CC) $(CFLAGS) -o server-rumprun server.c

clienta-rumprun: clienta.c 
	$(CC) $(CFLAGS) -o clienta-rumprun clienta.c

client-rumprun: client.c 
	$(CC) $(CFLAGS) -o client-rumprun client.c

dist_clean: clean
	rm $(BINS) 

clean:
	rm -f *.o server-rumprun client-rumprun clienta-rumprun

