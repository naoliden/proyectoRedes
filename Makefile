all:
  gcc -o server server.c damas.c
  gcc -o client client.c

server:
	gcc -o server server.c damas.c

client:
  gcc -o client client.c
