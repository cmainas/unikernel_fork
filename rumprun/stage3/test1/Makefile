CC = /path/to/x86_64-rumprun-netbsd-gcc
BK = /path/to/rumprun-bake

CFLAGS = -Wall

TARGET = hw_generic_iv

BINS = server-rumprun.bin client1-rumprun.bin client-rumprun.bin

all: $(BINS)

server-rumprun.bin: server-rumprun
	$(BK) $(TARGET) server-rumprun.bin server-rumprun

client1-rumprun.bin: client1-rumprun
	$(BK) $(TARGET) client1-rumprun.bin client1-rumprun

client-rumprun.bin: client-rumprun
	$(BK) $(TARGET) client-rumprun.bin client-rumprun

server-rumprun: server.c
	$(CC) $(CFLAGS) -o server-rumprun server.c

client1-rumprun: client1.c 
	$(CC) $(CFLAGS) -o client1-rumprun client1.c

client-rumprun: client.c 
	$(CC) $(CFLAGS) -o client-rumprun client.c

dist_clean: clean
	rm $(BINS) 

clean:
	rm -f *.o server-rumprun client-rumprun client1-rumprun

