# Get source files.
SRCS = $(shell find src -name '*.c')

# Set compiler flags.
CC = gcc
CFLAGS = -std=c11 -o anash -I'include' -g -O3 -D _GNU_SOURCE

all: anash

anash: $(SRC)
	$(CC) $(CFLAGS) $(SRCS)

clean:
	rm -f anash
