#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <unistd.h>

#define PORT 4455

int main(){
  int clientSocket;
  struct sockaddr_in serverAddr;
  char buffer[1024];
  clientSocket = socket(PF_INET, SOCK_STREAM, 0);
  printf("Client socket created successfully\n");
  memset(&serverAddr, '\0', sizeof(serverAddr));
  serverAddr.sin_family = AF_INET;
  serverAddr.sin_port = htons(PORT);
  serverAddr.sin_addr.s_addr = inet_addr("10.201.149.195");

  connect(clientSocket, (struct sockaddr *) &serverAddr, sizeof(serverAddr));
  printf("Connected to the server\n");
  recv(clientSocket, buffer, 1024, 0);
  printf("%s\n", buffer);
  strcpy(buffer, "\0");
  scanf("%s", buffer);
  send(clientSocket, buffer, 1024, 0);
  strcpy(buffer, "\0");
  recv(clientSocket, buffer, 1024, 0);
  printf("%s\n", buffer);

  printf("[+]Closing the connection\n");
}
