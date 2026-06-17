CC=gcc
CXX=g++

lseek_test: 06-lseek.c
	$(CC) $< -o $@
stat_test: 06-stat.c
	$(CC) $< -o $@
.PHONY: clean mkln_lseek
clean:
	rm -r lseek lseek_test stat_test
mkln_lseek:
	ln -s lseek lseek_test