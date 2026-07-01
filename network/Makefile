CC = gcc
targets = basic_client basic_server

basic_client: basic_socket_demo.c
	$(CC) -DCLIENT $< -o $@
basic_server: basic_socket_demo.c
	$(CC) $< -o $@

.PHONY: clean
clean: 
	rm -rf $(targets) *.o
