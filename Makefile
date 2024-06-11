CC=gcc
CFLAGS=-pthread
OBJECTS = Queue.c
HEADERS = Queue.h

.PHONY: all
all: randomGenerator primeCounter

randomGenerator: generator.c
	$(CC) -o randomGenerator generator.c

primeCounter: primeCounter.c Queue.c
	$(CC) $(CFLAGS) -o primeCounter primeCounter.c Queue.c

.PHONY: clean
clean:
	-rm randomGenerator primeCounter 2>/dev/null





