
BIN ?= clib-uninstall
PREFIX ?= /usr/local
SRC = $(wildcard src/*.c)
DEPS = $(wildcard deps/*/*.c)
OBJS = $(SRC:.c=.o) $(DEPS:.c=.o)
CFLAGS = -Ideps -std=c99 -Wall -Wextra
ifeq ($(OS),Windows_NT)
LDFLAGS = -lcurldll
else
LDFLAGS = -lcurl
endif

$(BIN): $(OBJS)
	$(CC) $^ -o $@ $(LDFLAGS)

install: $(BIN)
	cp -f $(BIN) $(PREFIX)/bin/

uninstall:
	rm -f $(PREFIX)/bin/$(BIN)

clean:
	rm -f $(OBJS) $(BIN)

test: $(BIN)
	./test.sh

.PHONY: clean install uninstall test
