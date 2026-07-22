# Native unit tests for hardware-independent firmware logic.
# Run locally or in CI:  make -C tests
CC      ?= cc
CFLAGS  := -std=gnu99 -Wall -Wextra -O1 -g
SRC     := ../src

TESTS := test_util

.PHONY: all clean
all: $(TESTS:%=run-%)

run-%: %
	@./$<

test_util: test_util.c $(SRC)/util.c $(SRC)/util.h
	$(CC) $(CFLAGS) -o $@ test_util.c $(SRC)/util.c -lm

clean:
	rm -f $(TESTS)
