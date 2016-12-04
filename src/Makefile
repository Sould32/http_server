
CC = gcc
CFLAGS = -Wall -O3 -Werror -pthread -std=gnu11
OBJS = sysstatd.o serve_static.o system_info.o artificial_loading.o

all: sysstatd

sysstatd: $(OBJS)
	$(CC) $(CFLAGS) -o sysstatd $(OBJS)

sysstatd.o: sysstatd.c sysstatd.h system_info.h

system_info.o: system_info.c system_info.h

artificial_loading.o: artificial_loading.c artificial_loading.h

serve_static.o: serve_static.c serve_static.h

clean:
	rm -f *~ *.o sysstatd
