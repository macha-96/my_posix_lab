CC=gcc

test: main.o MyThreadPool.o 
	$(CC) main.o MyThreadPool.o -o test
main.o: main.c MyThreadPool.h 
	$(CC) -c main.c -o main.o 
MyThreadPool.o: MyThreadPool.c MyThreadPool.h 
	$(CC) -c MyThreadPool.c -o MyThreadPool.o
clean: 
	rm -f *.o test
