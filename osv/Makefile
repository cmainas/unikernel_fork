CC = gcc

CFLAGS = -Wall -fPIC
LFLAGS += -shared
# CFLAGS += -O2 -fomit-frame-pointer -finline-functions

BINS = server.so client.so

all: $(BINS)

server.so: server.o
	$(CC) $(LFLAGS) $(CFLAGS) -o server.so server.o

server.o: server.c
	$(CC) $(CFLAGS) -c -o server.o server.c

client.so: client.o 
	$(CC) $(LFLAGS) $(CFLAGS) -o client.so client.o

client.o: client.c 
	$(CC) $(CFLAGS) -c -o client.o client.c

dist_clean: clean
	rm $(BINS) 

clean:
	rm -f *.o 
