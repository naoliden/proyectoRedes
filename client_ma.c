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
  buffer = (unsigned char)1;
  strcat(buffer, sizeof("hola"));
  strcat(buffer, "hola");
  send(clientSocket, buffer, strlen(buffer),0);

  printf("[+]Closing the connection\n");
}
