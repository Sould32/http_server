
CC = gcc
CFLAGS = -Wall -O3 -Werror -m32 -pthread -std=gnu11
OBJS = sysstatd.o

all: sysstatd

sysstatd: $(OBJS)
	$(CC) $(CFLAGS) -o sysstatd $(OBJS)

sysstatd.o: sysstatd.c sysstatd.h

clean:
	rm -f *~ *.o sysstatd
