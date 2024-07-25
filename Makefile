BIN = spycycles
SRC = spycycles.c
LIBS = -lcapstone
CFLAGS = -W -Wall
CC = gcc

.PHONY: all
all: install $(BIN)

$(BIN): $(SRC)
	$(CC) $(CFLAGS) $(SRC) $(LIBS) -o $(BIN)

.PHONY: install
install:
	sudo apt-get install libcapstone-dev

.PHONY: clean
clean:
	rm -f $(BIN)
