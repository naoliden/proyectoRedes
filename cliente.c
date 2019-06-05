#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include "math.h"
#include "client.h"



int initializeClient(char* ip, int port){
	int clientSocket;
  //char buffer[1024];
  struct sockaddr_in serverAddr;
  socklen_t addr_size;

  /*---- Creación del Socket. Se pasan 3 argumentos ----*/
	/* 1) Internet domain 2) Stream socket 3) Default protocol (TCP en este caso) */
	int iSetOption = 1;
  clientSocket = socket(PF_INET, SOCK_STREAM, 0);
  setsockopt(clientSocket, SOL_SOCKET, SO_REUSEADDR, (char*)&iSetOption, sizeof(iSetOption));
  /*---- Configuración de la estructura del servidor ----*/
	/* Address family = Internet */
  serverAddr.sin_family = AF_INET;
	/* Set port number */
  serverAddr.sin_port = htons(port);
  /* Setear IP address como localhost */
  serverAddr.sin_addr.s_addr = inet_addr(ip);
  /* Setear todos los bits del padding en 0 */
  memset(serverAddr.sin_zero, '\0', sizeof serverAddr.sin_zero);

  /*---- Conectar el socket al server ----*/
  addr_size = sizeof serverAddr;
  int ret = connect(clientSocket, (struct sockaddr *) &serverAddr, addr_size);
  if (ret == 0) printf("Connected to server!\n");
  else printf("Error connecting to server. Exit code: %d (probablemente el puerto está tomado. Solución: esperar unos segundos, cambiar el puerto o configurar el socket)\n", ret);
  // RECUERDEN MANEJAR ERRORES EN CADA FUNCIÓN EXTERNA!!

	return clientSocket;
}

Message * receiveMessage(int socket){
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


	Message * msg = malloc(sizeof(Message));
	msg->id = ID;
	msg->size = payloadSize;
	msg->payload = calloc(payloadSize, sizeof(char));

	memcpy(msg->payload, message, payloadSize);
	
	return msg;
	
  // Aqui se las ingenian para ver como retornan todo. Puden retornar el paquete y separarlo afuera, o retornar una struct.
}

void sendMessage(int socket, Message* msg){

  int size_buffer = 2 + msg -> size;
  char *buffer = calloc(size_buffer, sizeof(char));
  buffer[0] = msg -> id; // *(buffer) = ...
  buffer[1] = msg -> size; // *(buffer + 1) = ...
  memcpy(buffer + 2, msg -> payload, msg -> size);

  send(socket, buffer, size_buffer , 0);
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
  // Los primeros dos bytes los imprimimos como 'd' (números), porque así acordamos interpretarlos.
  printf("   Paquete es: ");
  printf("%d ", package[0]); printf("%d ", package[1]); printf("%s\n", &package[2]);

}

void handle_command(Message * mensaje, int socket){
	switch (mensaje->id){
		case 0:
			printf("testcase: %s", mensaje->id);
			break;
		case 19:
			printf("Mensaje de chat:\n");
			printf("%s", mensaje->payload);
			break;
		default:
			printf("commando invalido");
	}
}

void mensaje_chat(int socket){
	char input[255];
	printf("\nEnter your message: \n");
	scanf("%[^\n]%*c", input);
	printf("Ingresaste: %s\n", input);

	Message * msg = malloc(sizeof(Message));
	msg->size = 255;
	msg->payload = calloc(255, sizeof(char));
	memcpy(msg->payload, input, 255);
	msg->id = 19;

	sendMessage(socket, msg);
}


int main(int argc, char const *argv[])
{
	printf("I'm a Cliente\n");
    int socket = initializeClient(IP, PORT);
	char choice;

    do {
		printf("Menu\n\n");
		printf("1. Chat\n");
		printf("2. ---\n");
		printf("3. Exit\n");
		scanf("%d",&choice);
		
		switch (choice){
			case 1:
				mensaje_chat(socket);
				break;
			case 2:
				break;
			case 3:
				printf("Goodbye\n"); 
				break;
			default:
				printf("Wrong Choice. Enter again\n");
				break;
		} 
	} while (choice != 3);
	
	return 0;
}
