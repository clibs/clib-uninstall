
SRC     = $(wildcard src/*.c)
DEPS    = $(wildcard deps/*/*.c)
OBJS    = $(SRC:.c=.o)
OBJS   += $(DEPS:.c=.o)

CFLAGS  = -Ideps -std=c99 -Wall -Wextra
LDFLAGS = -lcurl

BIN    ?= clib-uninstall
PREFIX ?= /usr/local

$(BIN): src/main.o $(OBJS)
	$(CC) $^ -o $@ $(LDFLAGS)

%.o: %.c
	$(CC) $< -c -o $@ $(CFLAGS)

install: $(BIN)
	cp -f $(BIN) $(PREFIX)/bin/

uninstall:
	rm -f $(PREFIX)/bin/$(BIN)

clean:
	rm -f $(OBJS) $(BIN) src/main.o

test: $(BIN)
	./test.sh

.PHONY: clean install uninstall test
