all: client server

client:
	gcc -o client client.c

server:
	gcc -o server server.c
