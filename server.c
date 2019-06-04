#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include "math.h"

#include "server.h"

#define IP "0.0.0.0"
#define PORT 8080


int initializeServer(char* ip, int port, ){
	int welcomeSocket, newSocket;
	struct sockaddr_in serverAddr;
	struct sockaddr_storage serverStorage;
	socklen_t addr_size;

	/*---- Creación del Socket. Se pasan 3 argumentos ----*/
	/* 1) Internet domain 2) Stream socket 3) Default protocol (TCP en este caso) */
	welcomeSocket = socket(PF_INET, SOCK_STREAM, 0);

	/*---- Configuración de la estructura del servidor ----*/
	/* Address family = Internet */
	serverAddr.sin_family = AF_INET;
	/* Set port number */
	serverAddr.sin_port = htons(port);
	/* Setear IP address como localhost */
	serverAddr.sin_addr.s_addr = inet_addr(ip);
	/* Setear todos los bits del padding en 0 */
	memset(serverAddr.sin_zero, '\0', sizeof serverAddr.sin_zero);

	/*---- Bindear la struct al socket ----*/
	bind(welcomeSocket, (struct sockaddr *) &serverAddr, sizeof(serverAddr));

	return welcomeSocket;

	/*---- Listen del socket, con un máximo de 5 conexiones (solo como ejemplo) ----*/
	// if(listen(welcomeSocket,5)==0)
	// 	printf("Waiting for the second user to connect...\n");
	// else
	// 	printf("Error\n");

	// addr_size = sizeof serverStorage;
	// newSocket = accept(welcomeSocket, (struct sockaddr *) &serverStorage, &addr_size);
	// printf("Conected\n");
	// return newSocket;
}

char* recieveMessage(int socket, char* message){
	printf("Waiting message... ♔ \n");
	recv(socket, message, 1024, 0);
	return message;
}

void sendMessage(int socket, char* message){
	send(socket, message, 1024,0);
}

int main(){

	struct sockaddr_in serverAddr;
	struct sockaddr_storage serverStorage;
	serverSocket = initializeServer(IP, PORT, &serverAddr, &serverStorage);
	addr_size = sizeof serverStorage;
	newSocket = accept(welcomeSocket, (struct sockaddr *) &serverStorage, &addr_size);
	
	while(listen(serverSocket) < 2){
		printf("Waiting for the second user to connect...\n");

	}
	printf("Dos usuarios conectados");
	





}
