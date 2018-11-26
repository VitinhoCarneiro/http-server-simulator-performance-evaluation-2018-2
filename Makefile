CC = gcc
CFLAGS = -Wall -Wextra -Werror -O2

tests : tests/test_rng-bin tests/test_queue-bin

tests/test_rng-bin : tests/test_rng.c rng.o
	$(CC) $(CFLAGS) -o tests/test_rng-bin tests/test_rng.c rng.o

tests/test_queue-bin : tests/test_queue.c queue.o
	$(CC) $(CFLAGS) -o tests/test_queue-bin tests/test_queue.c queue.o

rng.o : rng.c rng.h
	$(CC) $(CFLAGS) -c rng.c

queue.o : queue.c queue.h
	$(CC) $(CFLAGS) -c queue.c

clean :
	rm -fv *.o tests/*-bin
