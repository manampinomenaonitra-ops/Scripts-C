CC=gcc
CFLAGS=-Iinclude -Wall -Wextra
SRC=src/server.c src/daemon.c src/handler.c
OBJ=$(SRC:.c=.o)
TARGET=myserverd

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CC) -o $@ $^ $(CFLAGS)

%.o: %.c
	$(CC) -c -o $@ $< $(CFLAGS)

clean:
	rm -f src/*.o $(TARGET)

fclean: clean
	rm -f /tmp/myserverd.pid

.PHONY: all clean fclean
