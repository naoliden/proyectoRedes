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


Conexiones initializeServer(char* ip, int port){
	int welcomeSocket, cliente1, cliente2;
	struct sockaddr_in serverAddr;
	struct sockaddr_storage serverStorage;
	socklen_t addr_size;

	Conexiones conexiones; 

	/*---- Creación del Socket. Se pasan 3 argumentos ----*/
	/* 1) Internet domain 2) Stream socket 3) Default protocol (TCP en este caso) */
	int iSetOption = 1;
	welcomeSocket = socket(PF_INET, SOCK_STREAM, 0);
	setsockopt(welcomeSocket, SOL_SOCKET, SO_REUSEADDR, (char*)&iSetOption, sizeof(iSetOption));
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
	if(listen(welcomeSocket,5)==0)
		printf("Waiting a client to connect...\n");
	else
		printf("Error\n");

	addr_size = sizeof serverStorage;
  // Servidor queda bloqueado aquí hasta que alguien se conecte.
	cliente1 = accept(welcomeSocket, (struct sockaddr *) &serverStorage, &addr_size);
	printf("Client 1 has connected to me!\n");

	Client c1;
	c1.clientSocket = cliente1;
	c1.connected = 1;
	c1.player = 1;

	cliente2 = accept(welcomeSocket, (struct sockaddr *) &serverStorage, &addr_size);
	printf("Client 2 has connected to me!\n");

	Client c2;
	c2.clientSocket = cliente2;
	c2.connected = 1;
	c2.player = 2;

	conexiones.p1 = c1;
	conexiones.p2 = c2;

	return conexiones;
}


Message * receiveMessage(int socket){

  // Esperamos a que llegue el primer byte, que corresponde al ID del paquete
	printf("Waiting message... \n");
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

	// char * mensaje = malloc(2 + payloadSize);

	Message * msg = malloc(sizeof(Message));
	msg->id = ID;
	msg->size = payloadSize;
	msg->payload = calloc(payloadSize, sizeof(char));

	memcpy(msg->payload, message, payloadSize);
	
	return msg;
}

void sendMessage(int socket, Message* msg){
  // Obtenemos el largo del payload para saber qué tamaño tiene el paquete y cuántos bytes debe enviar mi socket
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


void handle_command(Message * mensaje, Conexiones * conexiones){
	switch (mensaje->id){
		case 0:
			printf("testcase: %s", mensaje->id);
			break;
		case 19:
			printf("Mensaje de chat\n");
			if (mensaje->sender == 1){
				sendMessage(conexiones->p2.clientSocket, mensaje);
			} else {
				sendMessage(conexiones->p1.clientSocket, mensaje);
			}
			break;
		default:
			printf("commando invalido");
	}
}

int main(int argc, char const *argv[])
{
	Conexiones conexiones;
	conexiones = initializeServer(IP, PORT);
	
	Message * init_msg = malloc(sizeof(Message));
	init_msg->id = 19;
	init_msg->size = 10;
	init_msg->payload = calloc(11, sizeof(char));
	strcpy(init_msg->payload, "Bienvenido");
	sendMessage(conexiones.p1.clientSocket, init_msg);
	free(init_msg->payload);
	free(init_msg);

	// TODO NICKNAMES !

	int turno = 1;
    while (1) {
		if(turno == 1){
			printf("Turno 1\n");

			// Esperamos el mensaje del cliente
			Message * msg = receiveMessage(conexiones.p1.clientSocket);
			msg->sender = 1;
			// printf("mensaje: %s", msg->payload);
			
			handle_command(msg, &conexiones);
			
			
			
			// turno = 2;

			free(msg->payload);
			free(msg);

		}

		if(turno == 2){
			printf("Turno 2");

			// Esperamos el mensaje del cliente
			Message * msg = receiveMessage(conexiones.p2.clientSocket);
			msg->sender = 2;
			// printf("mensaje: %s", msg->payload);
			
			handle_command(msg, &conexiones);



			turno = 1;

			free(msg->payload);
			free(msg);

		}

		printf("\n\nSe ha completado una ronda\n\n");

	}
	return 0;
}
