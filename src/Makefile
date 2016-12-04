
CC = gcc
CFLAGS = -Wall -O3 -Werror -m32 -pthread -std=gnu11
OBJS = sysstatd.o system_info.o

all: sysstatd

sysstatd: $(OBJS)
	$(CC) $(CFLAGS) -o sysstatd $(OBJS)

sysstatd.o: sysstatd.c sysstatd.h system_info.h

system_info.o: system_info.c system_info.h

clean:
	rm -f *~ *.o sysstatd
