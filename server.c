#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>

#define PORT 4455


int main(){
  int welcomeSocket;
  struct sockaddr_in serverAddr;
  int c1;
  struct sockaddr_in newAddr_c1;
  int c2;
  struct sockaddr_in newAddr_c2;

  socklen_t addr_size, addr_size_2;
  char buffer[1024], nickname1[256], nickname2[256];

  //paramètrage du Serveur
  welcomeSocket = socket(PF_INET, SOCK_STREAM, 0);
  printf("[+]Server socket created successfully\n");
  memset(&serverAddr, '\0', sizeof(serverAddr));

  serverAddr.sin_family = AF_INET;
  serverAddr.sin_port = htons(PORT);
  serverAddr.sin_addr.s_addr = inet_addr("127.0.0.1");

  bind(welcomeSocket, (struct sockaddr *) &serverAddr, sizeof(serverAddr));
  printf("Bind to the port number %d\n", 4455);
  listen(welcomeSocket, 3);
  printf("Listening...\n");
  addr_size = sizeof(newAddr_c1);

  c1 = accept(welcomeSocket, (struct sockaddr*)&newAddr_c1, &addr_size);

  strcpy(buffer, "What is your nickname?");
  send(c1, buffer, strlen(buffer), 0);
  recv(c1, buffer, 1024, 0);
  printf("NICKNAME IS: %s\n", buffer);
  strcpy(nickname1, buffer);
  addr_size_2 = sizeof(newAddr_c2);

  c2 = accept(welcomeSocket, (struct sockaddr*)&newAddr_c2, &addr_size_2);

  strcpy(buffer, "What is your nickname?");
  send(c2, buffer, strlen(buffer), 0);
  strcpy(buffer, "\0");
  recv(c2, buffer, 1024, 0);
  printf("NICKNAME IS: %s\n", buffer);
  strcpy(nickname2, buffer);

  strcpy(buffer, "Found opponent: ");
  strcat(buffer, nickname2);
  send(c1, buffer, strlen(buffer), 0);
  strcpy(buffer, "\0");
  strcpy(buffer, "Found opponent: ");
  strcat(buffer, nickname1);
  send(c2, buffer, strlen(buffer), 0);

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
