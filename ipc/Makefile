CC=gcc
CXX=g++

target=signal simple_pipe rdfifo wrfifo rdshm_nosem wrshm_nosem rdshm wrshm

signal: 08-signal.c
	$(CC) $< -o $@
simple_pipe: 08-simple_pipe.c
	$(CC) $< -o $@
rdfifo: 08-simple_fifo.c
	$(CC) $< -o $@
wrfifo: 08-simple_fifo.c
	$(CC) -DENWR $< -o $@
rdshm_nosem: 08-shm_nosem.c
	$(CC) $< -o $@
wrshm_nosem: 08-shm_nosem.c
	$(CC) -DENWR $< -o $@
rdshm: 08-shm.c
	$(CC) $< -o $@
wrshm: 08-shm.c
	$(CC) -DENWR $< -o $@

.PHONY: clean
clean:
	rm -r $(target)