#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>

#define PORT 4455
int welcomeSocket, c1, c2;

int initializeServer(char* ip, int port){
  struct sockaddr_in serverAddr;
  struct sockaddr_in newAddr_c1;
  struct sockaddr_in newAddr_c2;

  socklen_t addr_size, addr_size_2;
  char buffer[1024], nickname1[256], nickname2[256];

  //paramètrage du Serveur
  welcomeSocket = socket(PF_INET, SOCK_STREAM, 0);
  printf("[+]Server socket created successfully\n");
  memset(&serverAddr, '\0', sizeof(serverAddr));

  serverAddr.sin_family = AF_INET;
  serverAddr.sin_port = htons(PORT);
  serverAddr.sin_addr.s_addr = inet_addr("10.201.149.195");


  bind(welcomeSocket, (struct sockaddr *) &serverAddr, sizeof(serverAddr));
  printf("Bind to the port number %d\n", 4455);
  listen(welcomeSocket, 2);
  printf("Listening...\n");
  addr_size = sizeof(newAddr_c1);

  c1 = accept(welcomeSocket, (struct sockaddr*)&newAddr_c1, &addr_size);
  c2 = accept(welcomeSocket, (struct sockaddr*)&newAddr_c2, &addr_size_2);

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

  // return msg;
  return message;
}

void start_game(int socket){
  char ID1, ID2;
  recv(c1, &ID1, 1, 0);
  if (ID1 == 1) {
    printf("Client 1 connected%s\n");
    char buffer[1024];
    buffer = (unsigned char)1;
    strcat(buffer, (unsigned char) sizeof("Connected"));
    strcat(buffer, "Connected");
    printf("buffer: %s\n", buffer);
    send(c1, buffer, sizeof("Connected")+2, 0);
  }

}

int main(){
  initializeServer("10.201.149.195", PORT);
  start_game(welcomeSocket);
/*PLAY GAME
  start_game();
  int no_winner = 1;
  while(no_winner){
    play_round();
    no_winner = winner_loser();
  }
  end_game();
*/


  printf("[+]Closing the connection\n");
}
