BIN = spycycles
SRC = spycycles.c
LIBS = -lcapstone
CFLAGS = -W -Wall
CC = gcc

.PHONY: default
default: $(BIN)

.PHONY: all
all: install default

$(BIN): $(SRC)
	$(CC) $(CFLAGS) $(SRC) $(LIBS) -o $(BIN)

.PHONY: install_deps
install_deps:
	sudo apt-get install libcapstone-dev

.PHONY: clean
clean:
	rm -f $(BIN)
