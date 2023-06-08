# Setup basic things
CC=gcc
CFLAGS=-std=c99 -Wall -Wextra

# Generate list of executables [usage: $(subst, pattern, replacement, text)]
EXECUTABLES:= $(subst  .c, , $(wildcard *.c))

# Create 'all' rule
all: $(EXECUTABLES)

# Create 'clean' rule
.PHONY: clean

clean:
	rm -rf *.o *~ $(EXECUTABLES)
