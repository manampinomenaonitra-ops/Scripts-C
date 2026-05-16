CC = gcc
CFLAGS = -Wall -Wextra -pthread -Iinclude
SRC = src/server.c src/daemon.c src/handler.c src/utils.c
OBJ = $(SRC:.c=.o)
EXEC = myserver

all: $(EXEC)

$(EXEC): $(OBJ)
	$(CC) $(CFLAGS) -o $@ $^

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f src/*.o $(EXEC)

