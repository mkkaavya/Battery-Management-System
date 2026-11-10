CC      = gcc
CFLAGS  = -std=c99 -Wall -Wextra -O2
SRC     = $(filter-out src/main.c, $(wildcard src/*.c))

all: bms
bms: src/main.c $(SRC)
	$(CC) $(CFLAGS) -o $@ $^
test: tests/test_bms.c $(SRC)
	$(CC) $(CFLAGS) -o test_bms $^ -lm && ./test_bms
run: bms
	./bms
clean:
	rm -f bms test_bms bms_log.txt
