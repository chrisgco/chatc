all: client server

client:
	gcc -o client client.c

server: support
	gcc -o server support.o server.c

support:
	gcc  -c -o support.o support.c
