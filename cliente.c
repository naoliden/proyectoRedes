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

int main(int argc, char const *argv[])
{
	printf("I'm a Client\n");
    int socket = initializeClient(IP, PORT);

    // Definimos un máximo de 10 intercambios para terminar el programa
    int count = 0;
    while (count<10) {
      // Pedimos un mensaje al cliente
      char input[100];
      printf("\nEnter your message: ");
      scanf("%s", input); // esta función termina la captura con el char nulo '\0' (0 si lo interpretas como número) cuando encuentra un espacio o salto de linea. Hay una decena de funciones similares. Investiguen!
      printf("    Ingresaste: %s\n", input);

      // Calculamos el largo del mensaje ingresado por el humano
      int msgLen = calculate_length(input); //no se debería enviar el caracter nulo al final del input. Ojo que al imprimir el string sin este caracter les aparecerá un simbolo raro al final

      // Armamos el paquete a enviar
      char package[2+msgLen];
      // Definimos el ID, el payloadSize y copiamos el mensaje
      package[0] = 19;
      package[1] = msgLen;
      strcpy(&package[2], input);

      // Imprimamos el paquete para ver cómo quedó
      print_package(package);
      
      // Enviamos el paquete a través del socket
      sendMessage(socket, package);
  
      // Esperamos el mensaje del servidor
      char * msg = receiveMessage(socket);
      //printf("%s\n", msg);
    }
	return 0;
}
