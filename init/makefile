CFLAGS ?= -g -O2 -pipe -Wall -Wextra

override CFLAGS += -std=gnu11

all: init

init: init.c
	$(CC) $(CFLAGS) $< -o $@

.PHONY: install
install: all
	install -d $(DESTDIR)$(PREFIX)/bin
	install init $(DESTDIR)$(PREFIX)/bin/init
