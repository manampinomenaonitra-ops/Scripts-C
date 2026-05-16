CC=gcc
CFLAGS=-Iinclude -Wall
OBJ=src/server.o src/daemon.o src/handler.o

all: myserverd inetd_server

myserverd: $(OBJ)
	$(CC) -o $@ $^ $(CFLAGS)

inetd_server: src/inetd_server.c
	$(CC) -o echoserver_inetd src/inetd_server.c $(CFLAGS)

%.o: %.c
	$(CC) -c -o $@ $< $(CFLAGS)

clean:
	rm -f src/*.o myserverd echoserver_inetd /tmp/myserverd.pid /tmp/f

.PHONY: all clean
