CC = gcc
CFLAGS = -Wall -Wextra -Werror -std=c99 -O2

tests : tests/test_rng-bin tests/test_queue-bin tests/test_rand_exp-bin

tests/test_rand_exp-bin : tests/test_rand_exp.c random_var.h random_var.o rng.o
	$(CC) $(CFLAGS) -o tests/test_rand_exp-bin tests/test_rand_exp.c random_var.o rng.o -lm

tests/test_rng-bin : tests/test_rng.c rng.h rng.o
	$(CC) $(CFLAGS) -o tests/test_rng-bin tests/test_rng.c rng.o

tests/test_queue-bin : tests/test_queue.c queue.h queue.o
	$(CC) $(CFLAGS) -o tests/test_queue-bin tests/test_queue.c queue.o

random_var.o: random_var.c random_var.h rng.h
	$(CC) $(CFLAGS) -c random_var.c

rng.o : rng.c rng.h
	$(CC) $(CFLAGS) -c rng.c

queue.o : queue.c queue.h
	$(CC) $(CFLAGS) -c queue.c

clean :
	rm -fv *.o tests/*-bin
