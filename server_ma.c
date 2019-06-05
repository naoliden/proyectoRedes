#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include "math.h"
#define IP "10.201.149.195"
#define PORT 8080

int c1, c2;
char nn1[256], nn2[256];

char* receiveMessage(int socket){
  printf("Waiting message... \n");
  // Esperamos a que llegue el primer byte, que corresponde al ID del paquete
  char ID;
  recv(socket, &ID, 1, 0);
  printf("\n####### Paquete recibido ####\n");
  printf("The ID is: %d\n", ID); // lo imprimimos como número porque esa es la interpretación que acordamos

  // Recibimos el payload size en el siguiente byte
  char payloadSize;
  recv(socket, &payloadSize, 1, 0);
  printf("The PayloadSize is: %d\n", payloadSize);

  // Recibimos el resto del paquete, según el payloadSize. Lo guardamos en un puntero de caracteres, porque no es necesario modificarlo
  char * message = malloc(payloadSize);
  recv(socket, message, payloadSize, 0);
  printf("The Message is: %s\n", message);
  printf("#############################\n");

  // Aqui se las ingenian para ver como retornan todo. Puden retornar el paquete y separarlo afuera, o retornar una struct.
  return message;
}
void sendMessage(int socket, char* package){
  int payloadSize = package[1];
  send(socket, package, 2 + payloadSize, 0);
}
int calculate_length(char * input){
  int i = 0;
  while (1){
    if (input[i] == '\0'){
      return i;
    }
    i++;
  }
}
void print_package(char * package){
  printf("   Paquete es: ");
  printf("%d ", package[0]); printf("%d ", package[1]); printf("%s\n", &package[2]);
}

void initializeServer(char* ip, int port){
  int welcomeSocket;
	struct sockaddr_in serverAddr;
	struct sockaddr_storage serverStorage;
	socklen_t addr_size;

	/*---- Creación del Socket. Se pasan 3 argumentos ----*/
	/* 1) Internet domain 2) Stream socket 3) Default protocol (TCP en este caso) */
	welcomeSocket = socket(PF_INET, SOCK_STREAM, 0);

  // Pueden buscar como configurar el socket para que libere el puerto una vez que se termine el programa

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

	/*---- Listen del socket, con un máximo de 5 conexiones (solo como ejemplo) ----*/
	if(listen(welcomeSocket,2)==0)
		printf("Waiting a client to connect...\n");
	else
		printf("Error\n");

	addr_size = sizeof serverStorage;
  // Servidor queda bloqueado aquí hasta que alguien se conecte.
	c1 = accept(welcomeSocket, (struct sockaddr *) &serverStorage, &addr_size);
	printf("Client has connected to me!\n");
  c2 = accept(welcomeSocket, (struct sockaddr *) &serverStorage, &addr_size);
	printf("Client 2 has connected to me!\n");
}

char* set_up(int client){
  char ID, payloadSize;
  char * message = malloc(payloadSize);
  int msgLen;
  // GET CONNECT MESSAGE
  recv(client, &ID, 1, 0);
  printf("The ID is: %d\n", ID);
  recv(client, &payloadSize, 1, 0);
  recv(client, message, payloadSize, 0);

  // SEND CONNECT MESSAGE
  msgLen = calculate_length("CONNECTION ESTABLISHED");
  char p[2+msgLen];
  p[0] = 2;
  p[1] = msgLen;
  strcpy(&p[2], "CONNECTION ESTABLISHED");
  print_package(p);
  sendMessage(client, p);

  // ASK FOR NICKNAME
  msgLen = calculate_length("What is your nickname?");
  char package[2+msgLen];
  package[0] = 3;
  package[1] = msgLen;
  strcpy(&package[2], "What is your nickname?");
  print_package(package);
  sendMessage(client, package);

  // GET NICKNAME C1
  recv(client, &ID, 1, 0);
  printf("The ID is: %d\n", ID);
  recv(client, &payloadSize, 1, 0);
  recv(client, message, payloadSize, 0);
  char nn[256];
  if(ID == 4){
    strcpy(nn, message);
    printf("NICKNAME: %s\n", nn);
  }
  return nn;
}

void send_nicknames(int client, char* nn){
  char nickname[256];
  strcpy(nickname, "Opponent found! Their nickname is: ");
  strcat(nickname, nn);
  printf("NN: %s\n", nickname);
  int msgLen = calculate_length(nickname);
  char package[2+msgLen];
  package[0] = 5;
  package[1] = msgLen;
  strcpy(&package[2], nickname);
  print_package(package);
  sendMessage(client, package);

}


int main (){
  int socket;
  printf("I'm the Server\n");
  initializeServer(IP, PORT);

  strcpy(nn1,set_up(c1));
  strcpy(nn2,set_up(c2));
  send_nicknames(c1, nn2);
  send_nicknames(c2, nn1);

  int count = 0;
  while (count<10) {
    char* msg = receiveMessage(c1);
    char input[100];
    printf("\nEnter your Message: ");
    scanf("%s", input);
    printf("\n");
    printf("   Ingresaste: %s\n", input);

    int msgLen = calculate_length(input);

    char package[2+msgLen];
    package[0] = 19;
    package[1] = msgLen;
    strcpy(&package[2], input);
    print_package(package);
    sendMessage(c1, package);
    }
  }
