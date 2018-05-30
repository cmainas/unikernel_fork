CC = gcc

CFLAGS = -Wall
CFLAGS += -g
# CFLAGS += -O2 -fomit-frame-pointer -finline-functions

LIBS = 

BINS = server client 

all: $(BINS)

server: server.o
	$(CC) $(CFLAGS) -o $@ $^ $(LIBS)

server.o: server.c
	$(CC) $(CFLAGS) -c $< $(LIBS)

client: client.o 
	$(CC) $(CFLAGS) -o $@ $^ $(LIBS) 

client.o: client.c 
	$(CC) $(CFLAGS) -c $< $(LIBS)

dist_clean: clean
	rm $(BINS) 

clean:
	rm -f *.o 
