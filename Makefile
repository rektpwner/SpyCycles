BIN = spycycles
SRC = spycycles.c
LIBS = -lcapstone
CFLAGS = -Wall -Wextra -pedantic -Werror
CC = gcc

.PHONY: default
default: $(BIN)

.PHONY: all
all: install_deps default

$(BIN): $(SRC)
	$(CC) $(CFLAGS) $(SRC) $(LIBS) -o $(BIN)

.PHONY: install_deps
install_deps:
	sudo apt-get install libcapstone-dev

.PHONY: clean
clean:
	rm -f $(BIN)
