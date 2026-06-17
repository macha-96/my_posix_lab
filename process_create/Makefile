CC = gcc
CXX = g++

target = echoall exec forkbase forkfd wait mine_shell_v2

echoall: 07-echoall.c
	$(CC) $< -o $@
exec: 07-exec.c
	$(CC) $< -o $@
forkbase: 07-forkbase.c
	$(CC) $< -o $@
forkfd: 07-forkfd.c
	$(CC) $< -o $@
wait: 07-wait.c
	$(CC) $< -o $@
mine_shell_v2: 07-mine_shell_v2.c
	$(CC) $< -o $@

.PHONY: clean
clean:
	rm -r $(target)