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


char * receiveMessage(int socket){

  // Esperamos a que llegue el primer byte, que corresponde al ID del paquete
	printf("Waiting message... \n");
	char ID;
	recv(socket, &ID, 1, 0);
	printf("\n####### Paquete recibido ####\n");
	printf("The ID is: %d\n", ID); // lo imprimimos como número porque esa es la interpretación que acordamos
	
	// Recibimos el payload size en el siguiente byte
	char payloadSize;
	recv(socket, &payloadSize, 1, 0);
	// printf("The PayloadSize is: %d\n", payloadSize);

	// Recibimos el resto del paquete, según el payloadSize. Lo guardamos en un puntero de caracteres, porque no es necesario modificarlo
	char * message = malloc(payloadSize);
	recv(socket, message, payloadSize, 0);
	// printf("The Message is: %s\n", message);
	printf("#############################\n");

	int size = atoi(payloadSize);
	printf("size: %d\n", size);

	char * mensaje = malloc(sizeof(char) * (2+size));
	// mensaje[0] = atoi(&ID);
	// mensaje[1] = size;

	// memcpy(&mensaje[0], &ID, 1);
	// memcpy(&mensaje[1], &payloadSize, 1);
	
	memcpy(mensaje, message, size);

	printf("mensjae %s\n", mensaje);
	// Aqui se las ingenian para ver como retornan todo. Puden retornar el paquete y separarlo afuera, o retornar una struct.
  	return mensaje;
}

void sendMessage(int socket, char* package){
  // Obtenemos el largo del payload para saber qué tamaño tiene el paquete y cuántos bytes debe enviar mi socket
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
  // Los primeros dos bytes los imprimimos como 'd' (números), porque así acordamos interpretarlos.
  printf("   Paquete es: ");
  printf("%d ", package[0]); printf("%d ", package[1]); printf("%s\n", &package[2]);
  
  /*
  // No hay diferencia entre un char o un int en la forma en que se guardan en memoria -> '01001110'
  // La diferencia es lo que representa, osea cómo yo lo interpreto y uso (letra o entero)
  for (int i=0; i<5; i++){
    // Si imprimimos cada caracter del mensaje como un numero, vemos su código ascii
    printf("%d ", package[2+i]);
  }
  printf("\n");
  */

}

void control(Message mensaje){
	switch (mensaje.id){
		case 19:
			printf("Mensaje de chat\n");
			break;
		default:
		printf("commando invalido");
	}
}

int main(int argc, char const *argv[])
{
	Conexiones conexiones;
	conexiones = initializeServer(IP, PORT);
	int turno = 1;

    while (1) {
		if(turno == 1){
			printf("Turno 1\n");

			// Esperamos el mensaje del cliente
			
			char * msg = receiveMessage(conexiones.p1.clientSocket);

			printf("funciona\n");

			printf("mensaje: %s", msg);

			// Pedimos un mensaje al servidor
			char input[100]; // definimos un arreglo estúpidamente grande
			printf("\nEnter your Message: ");

			scanf("%s", input);
			printf("\n");
			printf("   Ingresaste: %s\n", input);

			// Calculamos el largo del mensaje ingresado por el humano
			int msgLen = calculate_length(input); //no se debería enviar en el payload el caracter nulo al final del input. Ojo que al imprimir el string sin este caracter les aparecerá un simbolo raro al final

			// Armamos el paquete a enviar
			char package[2+msgLen];			
			package[0] = 19;
			package[1] = msgLen;
			strcpy(&package[2], input); //debería copiar hasta encontrar un caracter nulo, osea los msgLen caracteres

			// Imprimamos el paquete para ver cómo quedó
			print_package(package);
			sendMessage(conexiones.p1.clientSocket, package);

		}

		if(turno == 2){
			printf("Turno 2");
		}

	}
	return 0;
}
