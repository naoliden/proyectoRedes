#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include  <signal.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include "math.h"

#include "client.h"

// IP Anne
#define IP "146.155.158.222"
// #define IP "0.0.0.0"

// IP Norman
//#define IP "146.155.158.10"
#define PORT 8080


/* Función que inicializa el cliente en el port
con ip */
int initializeClient(char* ip, int port){
	int clientSocket;
  //char buffer[1024];
  struct sockaddr_in serverAddr;
  socklen_t addr_size;
  /*---- Creación del Socket. Se pasan 3 argumentos ----*/
	/* 1) Internet domain 2) Stream socket 3) Default protocol (TCP en este caso) */
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
  connect(clientSocket, (struct sockaddr *) &serverAddr, addr_size);
	printf("Connected to server!\n");
  /*---- Read the message from the server into the buffer ----*/

  unsigned int buffer_size;

  while(1){
    recv(clientSocket, buffer_size, 4, 0);
    printf("Mensaje size %d", buffer_size);
    sleep(1);
    unsigned char * buffer = malloc(sizeof(unsigned char) * buffer_size);
    recv(clientSocket, buffer, buffer_size, 0);


    // /*---- Print the received message ----*/
    // printf("Data received: %s",buffer);
    // strcpy(buffer,"Bye World\n");
    // send(clientSocket,buffer,11,0);

    free(buffer);
  }
  
	return clientSocket;
}


void move();

void send_message();

void listen();

void show_board();

void end_turn();

void chat_message();

void disconnect(){
  printf("Desconectandose del juego");
  // TODO enviar un mensaje de desconexion al servidor

  keepRunning = 0;
  exit(0);
}


static volatile sig_atomic_t keepRunning = 1;
char c;

static void intHandler(int _) {
  (void)_;
  printf("Presionaste Ctrl-C\n Quieres abandonar el juego? [y/n] ");
  c = getchar();
  if (c == 'y' || c == 'Y' || c == 's' || c == 'S'){
    disconnect();
  }
}


int main(int argc, char const *argv[]){

  signal(SIGINT, intHandler);
  
  initializeClient(IP, PORT);

  while(keepRunning){
    /* code */

  }
}
