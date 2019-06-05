#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <signal.h>
#include "client.h"

int clientSocket;
static volatile sig_atomic_t keep_running = 1;

int initializeClient(char* ip, int port){
  int clientSocket;
  struct sockaddr_in serverAddr;
  socklen_t addr_size;

  clientSocket = socket(PF_INET, SOCK_STREAM, 0);
  
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
  if (ret == 0){
  	printf("Connected to server!\n");
  } else {
	printf("Error connecting to server. Exit code: %d (probablemente el puerto está tomado. Solución: esperar unos segundos, cambiar el puerto o configurar el socket)\n", ret);
  }
  return clientSocket;
}

char* receiveMessage(int socket){
  printf("Waiting for message... \n");
  char ID;
  recv(socket, &ID, 1, 0);
  printf("\n####### Paquete recibido ####\n");
  printf("The ID is: %d\n", ID);
  
  char payloadSize;
  recv(socket, &payloadSize, 1, 0);
  printf("The PayloadSize is: %d\n", payloadSize);

  // Lo guardamos en un puntero de caracteres, porque no es necesario modificarlo
  char * message = malloc(payloadSize);
  recv(socket, message, payloadSize, 0);
  printf("The Message is: %s\n", message);
  printf("#############################\n");

  Message msg;
  msg.id = (int)ID;
  msg.size = (int)payloadSize;
  msg.payload = message;
  // return msg;
  return message;
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
  printf("%d ", package[0]);
  printf("%d ", package[1]);
  printf("%s\n", &package[2]);
  
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

static void sig_handler(int _){
    (void)_;
	printf("\nPresionaste Ctrl-C?\nQuieres salir? [y/n] ");
	char c = getchar();
	if (c == 'y' || c == 'Y'){
    	keep_running = 0;
		close(clientSocket);
		exit(0);
	}
}


int main(int argc, char const *argv[]){

    signal(SIGINT, sig_handler);
	// int clientSocket;
	clientSocket = initializeClient(IP, PORT);	
	while (keep_running) {
      char input[255];
      printf("\nEnter your message: ");
      scanf("%s", input); 
      printf("   Ingresaste: %s\n", input);

      int msgLen = calculate_length(input);
	  // Add two for id and size,
	  // review, maybe one byte for size is not enough
      char package[2+msgLen];
      // ID
      package[0] = 19;
	  // el payloadSize
      package[1] = msgLen;
	  // mensaje

      strcpy(&package[2], input);
      print_package(package);
      sendMessage(socket, package);
      
      // Esperamos el mensaje del servidor
      char * msg = receiveMessage(socket);
    //   printf("%s\n", msg);
    }
	return 0;
}