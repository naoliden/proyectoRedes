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
#define TRUE 1
int* initializeServer(char* ip, int port){
	int welcomeSocket, client1, client2;
	struct sockaddr_in serverAddr;
	struct sockaddr_storage serverStorage;
	socklen_t addr_size;
	int opt = TRUE;
	/*---- Creación del Socket. Se pasan 3 argumentos ----*/
	/* 1) Internet domain 2) Stream socket 3) Default protocol (TCP en este caso) */
	welcomeSocket = socket(PF_INET, SOCK_STREAM, 0);

	if (welcomeSocket == 0) {
		perror("SOCKET FAILED");
		exit(EXIT_FAILURE);
	}
	if (setsockopt(welcomeSocket, SOL_SOCKET, SO_REUSEADDR, (char * ) &opt, sizeof(opt)) < 0) {
		perror("setsockopt");
		exit(EXIT_FAILURE);
	}

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
	if(bind(welcomeSocket, (struct sockaddr *) &serverAddr, sizeof(serverAddr)) < 0){
		perror("bind failed");
		exit(EXIT_FAILURE);
	}


	/*---- Listen del socket, con un máximo de 5 conexiones (solo como ejemplo) ----*/
	if(listen(welcomeSocket,2)==0){
		printf("Waiting for the users to connect...\n");
	}
	else{
		perror("listen");
		exit(EXIT_FAILURE);
	}

	addr_size = sizeof serverStorage;
	client1 = accept(welcomeSocket, (struct sockaddr *) &serverStorage, &addr_size);
	client2 = accept(welcomeSocket, (struct sockaddr *) &serverStorage, &addr_size);
	printf("CLIENT: %d\n", client1);
	printf("CLIENT: %d\n", client2);
	printf("Conected\n");
	int * clients;
	clients[0] = client1;
	clients[1] = client2;
	return clients;
}

void send_message(char id, char size, char* msg){}

void check_valid_move(){}

void send_board(){}

void next_turn(){}

void winner_loser(){}

void ask_game(){}

void show_score(){}

void make_move(){}

int main(){
	int* c = malloc(2*sizeof(int));
	c = initializeServer("127.0.0.1", 8080);
	//ask_nickname();
	// get nicknames
	// send nicknames to opponents
	//start_game();
	//int no_victory = 1;
	//while(no_victory){
	//	play_round();
//		no_victory = winner_loser();
//	}

}
