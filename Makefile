# Get source files.
SRCS = $(shell find src -name '*.c')

# Set compiler flags.
CC = gcc
CFLAGS = -Wall -std=c11 -o anash -I'include' -g -O3 -D _GNU_SOURCE

all: anash

anash: $(SRCS)
	$(CC) $(CFLAGS) $(SRCS)

install: anash
	cp anash /usr/local/bin

clean:
	rm -f anash
