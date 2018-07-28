CC = /path/to/x86_64-rumprun-netbsd-gcc
BK = /path/to/rumprun-bake

CFLAGS = -Wall

TARGET = hw_generic_iv

BINS = test-rumprun.bin 

all: $(BINS)

test-rumprun.bin: test-rumprun
	$(BK) $(TARGET) test-rumprun.bin test-rumprun

test-rumprun: test.c
	$(CC) $(CFLAGS) -o test-rumprun test.c

dist_clean: clean
	rm $(BINS) 

clean:
	rm -f *.o test-rumprun 

