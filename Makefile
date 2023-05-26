CC=gcc
CFLAGS=-O2 -Wall -g -I/home/shuo/Programs/papi-7.0.1/src

all:	stride

stride:	stride2.c
	$(CC) $(CFLAGS) -o stride stride2.c /home/shuo/Programs/papi-7.0.1/src/libpapi.a

clean:
	rm  stride
