#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <signal.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include "math.h"

#include "client.h"


static volatile sig_atomic_t keepRunning = 1;
char c;

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
 
	return clientSocket;
}


void escuchar(int clientSocket){
   
  /*---- Read the message from the server into the buffer ----*/

  unsigned char * buffer_size;
  unsigned char type_aux[3];
  int msg_type;

  while(1){

    recv(clientSocket, type_aux, sizeof(unsigned char) * 2, 0);
    sleep(1);
    
<<<<<<< HEAD
    msg_type = (unsigned int)type_aux * 65536 + (unsigned int)type_aux * 256 + (unsigned int)type_aux;
=======
    msg_type = ((unsigned int)type_aux * 256 + (unsigned int)type_aux);
>>>>>>> b81bb3218b7f0ae651ad5158f18b165dcfc3e04d

    recv(clientSocket, buffer_size, 4, 0);
    printf("Mensaje size %s", buffer_size);
    sleep(1);

    unsigned char * buffer = malloc(sizeof(unsigned char) * 10);
    // recv(clientSocket, buffer, buffer_size, 0);

    switch (msg_type){
      case 0:
        printf("Testing buffer:\n%s", buffer);
      case 2:
        printf("Conexion al servidor exitosa");
        break;
      case 3:
        /* code */
        break;
      case 5:
        /* code */
        break;
      case 6:
        /* code */
        break;
      default:
        printf("Id de mensaje invalido. Id: %d", msg_type);
        break;
    }

    // /*---- Print the received message ----*/
    printf("Data received: %s",buffer);
    // strcpy(buffer,"Bye World\n");
    // send(*clientSocket,buffer,11,0);

    free(buffer);
  }
}

void move();

void send_message(int clientSocket, char * mensaje){
    send(clientSocket, mensaje, 255,0);
}

void show_board();

void end_turn();

void chat_message();

void write_log(char action);

void disconnect(){
  printf("Desconectandose del juego\n");
  keepRunning = 0;
  exit(0);
}


void intHandler(int _) {
  (void)_;
  printf("Presionaste Ctrl-C\n Quieres abandonar el juego? [y/n] ");
  c = getchar();
  if (c == 'y' || c == 'Y' || c == 's' || c == 'S'){
    disconnect();
  }
}


int main(int argc, char const *argv[]){

  signal(SIGINT, intHandler);
  int clientSocket = initializeClient(IP, PORT);
  printf("conectado?\n");
  // send_message(clientSocket, "hola");
  while(keepRunning){
    // escuchar(clientSocket);

  }
}
