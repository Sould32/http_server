
CC = gcc
CFLAGS = -Wall -O3 -Werror -pthread -std=gnu11
OBJS = sysstatd.o serve_static.o

all: sysstatd

sysstatd: $(OBJS)
	$(CC) $(CFLAGS) -o sysstatd $(OBJS)

sysstatd.o: sysstatd.c sysstatd.h

serve_static.o: serve_static.c serve_static.h

clean:
	rm -f *~ *.o sysstatd
