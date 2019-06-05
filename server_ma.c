#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include "math.h"
#include "damas.h"

#define IP "10.201.149.195"
#define PORT 4455

typedef struct mail{
  char id;
  char length;
  char* msg;
}mail;

int c1, c2;
char nn1[256], nn2[256];
Game* mi_juego;
mail * innbox_mail;
int innbox;


mail* create_mail(){
  mail* new = malloc(sizeof(mail));
  new->msg = malloc(512*sizeof(char));
  return new;
}

mail* receiveMessage(int socket){
  printf("Waiting message... \n");

  char ID;
  recv(socket, &ID, 1, 0);
  printf("\n####### Paquete recibido ####\n");
  printf("The ID is: %d\n", ID); // lo imprimimos como número porque esa es la interpretación que acordamos

  char payloadSize;
  recv(socket, &payloadSize, 1, 0);
  printf("The PayloadSize is: %d\n", payloadSize);

  char * messa = malloc(payloadSize);
  recv(socket, messa, payloadSize, 0);
  printf("The Message is: %s\n", messa);

  mail * m = create_mail();
  m->id = ID;
  m->length = payloadSize;
  strcpy(m->msg, messa);

  return m;
}
void sendMessage(int socket, char* package){
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
  printf("   Paquete es: ");
  printf("%d ", package[0]); printf("%d ", package[1]); printf("%s\n", &package[2]);
}
void initializeServer(char* ip, int port){
  int welcomeSocket;
	struct sockaddr_in serverAddr;
	struct sockaddr_storage serverStorage;
	socklen_t addr_size;

	/*---- Creación del Socket. Se pasan 3 argumentos ----*/
	/* 1) Internet domain 2) Stream socket 3) Default protocol (TCP en este caso) */
	welcomeSocket = socket(PF_INET, SOCK_STREAM, 0);

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
	if(listen(welcomeSocket,2)==0)
		printf("Waiting a client to connect...\n");
	else
		printf("Error\n");

	addr_size = sizeof serverStorage;
  // Servidor queda bloqueado aquí hasta que alguien se conecte.
	c1 = accept(welcomeSocket, (struct sockaddr *) &serverStorage, &addr_size);
	printf("Client has connected to me!\n");
  c2 = accept(welcomeSocket, (struct sockaddr *) &serverStorage, &addr_size);
	printf("Client 2 has connected to me!\n");
}
char* set_up(int client){
  char ID, payloadSize;
  char * mess = malloc(payloadSize);
  int msgLen;
  // GET CONNECT MESSAGE
  mail * m = create_mail();
  m = receiveMessage(client);
  printf("The id is %d\n", m->id);

  // SEND CONNECT MESSAGE
  msgLen = calculate_length("CONNECTION ESTABLISHED");
  char p[2+msgLen];
  p[0] = 2;
  p[1] = msgLen;
  strcpy(&p[2], "CONNECTION ESTABLISHED");
  print_package(p);
  sendMessage(client, p);

  // ASK FOR NICKNAME
  msgLen = calculate_length("What is your nickname?");
  char package[2+msgLen];
  package[0] = 3;
  package[1] = msgLen;
  strcpy(&package[2], "What is your nickname?");
  print_package(package);
  sendMessage(client, package);

  // GET NICKNAME C1
  mail* nickname = create_mail();
  nickname = receiveMessage(client);
  printf("The ID is: %d\n", nickname->id);
  char nn[256];
  if(nickname->id == 4){
    strcpy(nn, nickname->msg);
    printf("NICKNAME: %s\n", nn);
  }
  free(mess);
  free(m);
  free(nickname);
  return nn;
}
void send_nicknames(int client, char* nn){
  char nickname[256];
  strcpy(nickname, "Opponent found! Their nickname is: ");
  strcat(nickname, nn);
  printf("NN: %s\n", nickname);
  int msgLen = calculate_length(nickname);
  char package[2+msgLen];
  package[0] = 5;
  package[1] = msgLen;
  strcpy(&package[2], nickname);
  print_package(package);
  sendMessage(client, package);
}

void send_menu(int socket){
  int msgLen = calculate_length("1. Send chat message\n2. View board\n3. View Score\n4. Play move\n5. exit\n");
  char p[2+msgLen];
  p[0] = 21;
  p[1] = msgLen;
  strcpy(&p[2], "1. Send chat message\n2. View board\n3. View Score\n4. Play move\n5. Exit\n");
  sendMessage(socket, p);
}

void do_turn(int socket, char * my_nn, char * your_nn ){

  send_menu(socket);

  mail * answer = create_mail();
  answer = receiveMessage(socket);
  printf("id: %d, msg: %s\n", answer->id, answer->msg);

  if(answer->id == (unsigned char)19){
    innbox_mail->msg = answer->msg;
    innbox_mail->length = *answer->msg;
    innbox = 1;

    do_turn(socket, my_nn, your_nn);
  }
  else if(answer->id == (unsigned char)9){
    char * boardstring = malloc(64*sizeof(char));
    int index = 0;
    for (int i = 0;i<8;i++){
      for (int j = 0;j<8;j++){
        boardstring[index] = mi_juego->board[i][j];
        index++;
      }
    }
    char bord[66];
    bord[0] = 9;
    bord[1] = 64;
    memcpy(&bord[2], boardstring, 64);
    sendMessage(socket, bord);
    do_turn(socket, my_nn, your_nn);
  }
  else if(answer->id == (unsigned char)10){
    int v = 0;
    int row1, col1, row2, col2;
    char mesg[4];
    mesg[0] = answer->msg[0];
    mesg[0] = answer->msg[1];
    mesg[0] = answer->msg[2];
    mesg[0] = answer->msg[3];
    while (v != 12){
      if(v != 0){
        int msgLen = calculate_length("Invalid move. Please try again\n");
        char invalid[2+msgLen];
        invalid[0] = 11;
        invalid[1] = msgLen;
        strcpy(&invalid[2], "Invalid move. Please try again\n");
        sendMessage(socket, invalid);

        mail* new_move = create_mail();
        new_move = receiveMessage(socket);
        mesg[0] = new_move->msg[0];
        mesg[1] = new_move->msg[1];
        mesg[2] = new_move->msg[2];
        mesg[3] = new_move->msg[3];
      }
      row1 = mesg[0];
      col1 = mesg[1];
      row2 = mesg[2];
      col2 = mesg[3];
      printf("ROW: %d, COL: %d, ROW: %d, COL: %d\n", row1, col1, row2, col2 );
      v = do_move(mi_juego, row1, col1, row2, col2);
    }
    int msgLen = calculate_length("Valid move.\n");
    char valid[2+msgLen];
    valid[0] = 12;
    valid[1] = msgLen;
    strcpy(&valid[2], "Valid move.\n");
    sendMessage(socket, valid);
  }
  else if(answer->id == (unsigned char)7){
    int num_blanco = mi_juego->c_piezaso;
    int num_negro = mi_juego->c_piezasx;
    char score[256];
    strcpy(score, "Number of white left:\n");
    strcat(score, (char*)num_blanco);
    strcat(score, "\nNumber of black left:\n");
    strcat(score, (char*)num_negro);
    strcat(score, "\n");
    int msgLen = calculate_length(score);
    char scores[2+msgLen];
    scores[0] = 7;
    scores[1] = msgLen;
    strcpy(&scores[2], score);
    sendMessage(socket, scores);
    do_turn(socket, my_nn, your_nn);
  }


}

int main (){

  printf("I'm the Server\n");
  initializeServer(IP, PORT);

  // PREPARE FOR GAME
  strcpy(nn1,set_up(c1));
  strcpy(nn2,set_up(c2));
  send_nicknames(c1, nn2);
  send_nicknames(c2, nn1);

  // START GAME
  int msgLeng = calculate_length("Time to start playing");
  char start_playing[2+msgLeng];
  start_playing[0] = 6;
  start_playing[1] = msgLeng;
  strcpy(&start_playing[2], "Time to start playing");
  sendMessage(c1, start_playing);
  sendMessage(c2, start_playing);

  // WHOS FIRST
  char you_start[3];
  you_start[0] = 8;
  you_start[1] = 1;
  strcpy(&start_playing[2], "1");
  sendMessage(c1, you_start);

  char you_dont[3];
  you_dont[0] = 8;
  you_dont[1] = 1;
  strcpy(&start_playing[2], "2");
  sendMessage(c2, you_dont);


  // PLAY GAME
  mi_juego = init_game();
  int socket, msgLen;
  innbox = 0;
  int turn = 0;
  innbox_mail = create_mail();
  char * my_nn = malloc(256*sizeof(char));
  char * your_nn = malloc(256*sizeof(char));

  int no_winner = 1;
  while(no_winner){
    if(turn % 2 == 0){
      socket = c1;
      my_nn = nn1;
      your_nn = nn2;
    }
    else{
      socket = c2;
      my_nn = nn2;
      your_nn = nn1;
    }
    printf("It is %s's turn! Innbox is: %d\n", my_nn, innbox);

    if(innbox == 1){
      char * chat_message = malloc(512*sizeof(char));
      strcpy(chat_message, my_nn);
      strcat(chat_message, ", you have received a chat from ");
      strcat(chat_message, your_nn);
      strcat(chat_message, ": ");
      strcat(chat_message, innbox_mail->msg);
      msgLen = calculate_length(chat_message);
      char p[2+msgLen];
      p[0] = 19;
      p[1] = msgLen;
      strcpy(&p[2], chat_message);
      sendMessage(socket, p);
      innbox = 0;
    }

    do_turn(socket, my_nn, your_nn);

    no_winner = !mi_juego->termino;
    turn++;
  }
}
