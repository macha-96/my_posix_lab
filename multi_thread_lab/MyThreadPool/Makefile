CC=gcc
CFLAGS=-Wall -Wextra -pthread

test: main.o MyThreadPool.o 
	$(CC) $(CFLAGS) main.o MyThreadPool.o -o test
main.o: main.c MyThreadPool.h 
	$(CC) $(CFLAGS) -c main.c -o main.o 
MyThreadPool.o: MyThreadPool.c MyThreadPool.h 
	$(CC) $(CFLAGS) -c MyThreadPool.c -o MyThreadPool.o
clean: 
	rm -f *.o test