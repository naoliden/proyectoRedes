#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/time.h>
#include "math.h"
#include "damas.h"

#define IP "10.201.157.249"
#define PORT 8080

typedef struct loggen{
  char * big_log;
  int index;
}loggen;

typedef struct mail{
  char id;
  char length;
  char* msg;
}mail;

int c1, c2;
char nn1[256], nn2[256], nn[256];
char o, x;
Game* mi_juego;
mail * innbox_mail;
loggen * the_log;
int innbox;
int iSetOption = 1;

char modulo(char a, char b){
  char m = a%b;
  if (m<0){
    m = (b<0) ? m-b:m+b;
  }
  return m;
}

void create_log(){
  the_log = malloc(sizeof(loggen));
  the_log->big_log = malloc(4096*sizeof(char));
  the_log->index = 0;
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
void create_log_entry(char * action){
  //get timestamp
  char timestamp[30];
  struct timeval tv;
  time_t curtime;

 gettimeofday(&tv, 0);
 curtime=tv.tv_sec;
 strftime(timestamp,30,"%m-%d-%Y %T.",localtime(&curtime));

 // What happened?
 char entry[256];
 strcpy(entry, timestamp);
 strcat(entry, action);
 the_log->index = the_log->index + strlen(entry);
 printf("LOG ENTRY: %s\n", entry);
 memcpy(&the_log->big_log[the_log->index - strlen(entry)], entry, strlen(entry));
}
void sendMessage(int socket, char* package){
  int payloadSize = package[1];
  create_log_entry(package);
  send(socket, package, 2 + payloadSize, 0);
}
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

  while(ID <0 && ID > 21){
    int msgLen = calculate_length("PACKAGE ID INVALID");
    char p[2+msgLen];
    p[0] = 18;
    p[1] = msgLen;
    strcpy(&p[2], "PACKAGE ID INVALID");
    sendMessage(socket, p);
    recv(socket, &ID, 1, 0);
  }

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

  char save[2+m->length];
  save[0] = m->id;
  save[1] = m->length;
  strcpy(&save[2], m->msg);
  create_log_entry(save);

  return m;
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
  setsockopt(welcomeSocket, SOL_SOCKET, SO_REUSEADDR, (char*)&iSetOption, sizeof(iSetOption));
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
  sendMessage(client, p);

  // ASK FOR NICKNAME
  msgLen = calculate_length("What is your nickname?");
  char package[2+msgLen];
  package[0] = 3;
  package[1] = msgLen;
  strcpy(&package[2], "What is your nickname?");
  sendMessage(client, package);

  // GET NICKNAME C1
  mail* nickname = create_mail();
  nickname = receiveMessage(client);
  printf("The ID is: %d\n", nickname->id);
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
  sendMessage(client, package);
}
void send_scores(){
  int num_blanco = mi_juego->c_piezaso;
  int num_negro = mi_juego->c_piezasx;
  printf("O: %d, X: %d\n", num_blanco, num_negro);
  char blanco[100];
  char negro[100];
  sprintf(blanco, "%d", num_blanco);
  sprintf(negro, "%d", num_negro);
  char score[1024];
  strcpy(score, "Number of white left: ");
  strcat(score, blanco);
  strcat(score, "\nNumber of black left: ");
  strcat(score, negro);
  strcat(score, "\n");
  printf("%s\n", score);
  int msgLen = calculate_length(score);
  char scores[2+msgLen];
  scores[0] = 7;
  scores[1] = msgLen;
  strcpy(&scores[2], score);
  sendMessage(c1, scores);
  sendMessage(c2, scores);
}
void send_board(int socket){
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

}
void send_menu(int socket){
  int msgLen = calculate_length("Do you want to send a chat message? [1=y/2=n]");
  char p[2+msgLen];
  p[0] = 21;
  p[1] = msgLen;
  strcpy(&p[2], "Do you want to send a chat message? [1=y/2=n]");
  sendMessage(socket, p);
}
int killed_someone(int socket, int num_opponents){
  if (socket == c1 && num_opponents != mi_juego->c_piezasx){
    return 1;
  }
  else if(socket == c2 && num_opponents != mi_juego->c_piezaso){
    return 1;
  }
  else{
    return 0;
  }


}
void make_move(int socket, char * my_nn, char * your_nn ){

  mail * answer = create_mail();
  answer = receiveMessage(socket);

  if(answer->id == 19 && answer->length == 0){
    printf("NO NEW MESSAGE\n");
  }
  if(answer->id == 19 && answer->length>0){
    innbox_mail->msg = answer->msg;
    innbox_mail->length = answer->length;
    innbox = 1;
  }
  else if(answer->id == 17){
    int len = calculate_length("Game ended by one of the players");
    char quit[2+len];
    quit[0] = 21;
    quit[1] = len;
    strcpy(&quit[2], "Game ended by one of the players");
    sendMessage(c1, quit);
    sendMessage(c2, quit);
  }
  // SEND BOARD
  send_board(socket);
  // GET MOVE
  mail * get_move = create_mail();
  get_move = receiveMessage(socket);
  // CHECK IF VALID MOVE
  int v = 0;

  int row1, col1, row2, col2;
  row1 = (int)get_move->msg[0] -1;
  col1 = (int)get_move->msg[1] -1;
  row2 = (int)get_move->msg[2] -1;
  col2 = (int)get_move->msg[3] -1;

  int num_opponents;
  if(socket == c1){
    num_opponents = mi_juego->c_piezasx;
  }
  else{
    num_opponents = mi_juego->c_piezaso;
  }
  int counter = 1;
  while (v != 12 | counter == 1){
    printf("ROW: %d, COL: %d, ROW: %d, COL: %d\n", row1, col1, row2, col2 );
    v = do_move(mi_juego, row1, col1, row2, col2);
    printf("V=%d\n", v);
    counter = killed_someone(socket, num_opponents);
    printf("COUNTER [1-killed/0-not killed]: %d\n", counter);

    // IF INVALID MOVE
    if(v == 11){
      int msgLen = calculate_length("Invalid move. Please try again\n");
      char invalid[2+msgLen];
      invalid[0] = 11;
      invalid[1] = msgLen;
      strcpy(&invalid[2], "Invalid move. Please try again\n");
      sendMessage(socket, invalid);

      // SEND BOARD AGAIN
      send_board(socket);

      // GET NEW MOVE
      mail* new_move = create_mail();
      new_move = receiveMessage(socket);
      row1 = (int)new_move->msg[0] -1;
      col1 = (int)new_move->msg[1] -1;
      row2 = (int)new_move->msg[2] -1;
      col2 = (int)new_move->msg[3] -1;
    }
    // IF VALID MOVE AND KILLED SOMEONE
    else if(v == 12 && counter == 1){
      num_opponents--;
      int len = calculate_length("K");
      char killedit[2+len];
      killedit[0] = 12;
      killedit[1] = len;
      strcpy(&killedit[2], "K");
      sendMessage(socket, killedit);

      // SEND BOARD AGAIN
      send_board(socket);

      // GET NEW MOVE
      mail* new_move = create_mail();
      new_move = receiveMessage(socket);
      row1 = (int)new_move->msg[0] -1;
      col1 = (int)new_move->msg[1] -1;
      row2 = (int)new_move->msg[2] -1;
      col2 = (int)new_move->msg[3] -1;
    }
  }
  int msgLen = calculate_length("Valid move.\n");
  char valid[2+msgLen];
  valid[0] = 12;
  valid[1] = msgLen;
  strcpy(&valid[2], "Valid move.\n");
  sendMessage(socket, valid);
}
void play_game(){

  // GET READY TO START GAME
  char letsplay[] = "Time to start playing.\nYou are: O";
  int msgLeng = calculate_length(letsplay);
  char start_playing_1[2+msgLeng];
  start_playing_1[0] = 6;
  start_playing_1[1] = msgLeng;
  strcpy(&start_playing_1[2], letsplay);
  sendMessage(c1, start_playing_1);

  char letsgo[] = "Time to start playing.\nYou are: X";
  msgLeng = calculate_length(letsgo);
  char start_playing_2[2+msgLeng];
  start_playing_2[0] = 6;
  start_playing_2[1] = msgLeng;
  strcpy(&start_playing_2[2], letsgo);
  sendMessage(c2, start_playing_2);

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

      // WHO's TURN
      printf("SENDING WHOS TURN IT IS\n");
      char you_start[2 + calculate_length("1")];
      you_start[0] = 8;
      you_start[1] = calculate_length("1");
      strcpy(&you_start[2], "1");
      printf("SENDTE DETTE: %s\n", you_start);
      sendMessage(c1, you_start);

      char you_dont[2 + calculate_length("2")];
      you_dont[0] = 8;
      you_dont[1] = calculate_length("2");
      strcpy(&you_dont[2], "2");
      printf("SENDTE DETTE: %s\n", you_dont);
      sendMessage(c2, you_dont);
    }
    else{
      socket = c2;
      my_nn = nn2;
      your_nn = nn1;

      // WHO's TURN
      printf("SENDING WHOS TURN IT IS\n");
      char you_start[2 + calculate_length("1")];
      you_start[0] = 8;
      you_start[1] = calculate_length("1");
      strcpy(&you_start[2], "1");
      printf("SENDTE DETTE: %s\n", you_start);
      sendMessage(c2, you_start);

      char you_dont[2 + calculate_length("2")];
      you_dont[0] = 8;
      you_dont[1] = calculate_length("2");
      strcpy(&you_dont[2], "2");
      printf("SENDTE DETTE: %s\n", you_dont);
      sendMessage(c1, you_dont);
    }

    //SEND SCORE TO BOTH
    send_scores();


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

    // ASK ABOUT CHAT
    send_menu(socket);
    make_move(socket, my_nn, your_nn);

    no_winner = !mi_juego->termino;
    turn++;
  }

}
void end_game(){
  // SEND GAME OVER
  int msgLen = calculate_length("GAME OVER");
  char package[2+msgLen];
  package[0] = 13;
  package[1] = msgLen;
  strcpy(&package[2], "GAME OVER");
  sendMessage(c1, package);
  sendMessage(c2, package);

  // SEND BOARDSTATE
  send_board(c1);
  send_board(c2);

  // WINNER LOSER
  int winner = mi_juego->winner;
  if(winner == 1){
    char win_lose[256];
    strcpy(win_lose, "WINNER: ");
    strcat(win_lose, nn2);
    strcat(win_lose, "\nLOSERt: ");
    strcat(win_lose, nn1);
    strcat(win_lose, "\n");
    printf("%s\n", win_lose);
    int msgLen = calculate_length(win_lose);
    char winlose[2+msgLen];
    winlose[1] = msgLen;
    strcpy(&winlose[2], win_lose);
    winlose[0] = 14;
    sendMessage(c1, winlose);
    sendMessage(c2, winlose);

  }
  else if(winner == 2){
    char win_lose[256];
    strcpy(win_lose, "WINNER: ");
    strcat(win_lose, nn1);
    strcat(win_lose, "\nLOSERt: ");
    strcat(win_lose, nn2);
    strcat(win_lose, "\n");
    printf("%s\n", win_lose);
    int msgLen = calculate_length(win_lose);
    char winlose[2+msgLen];
    winlose[1] = msgLen;
    strcpy(&winlose[2], win_lose);
    winlose[0] = 14;
    sendMessage(c1, winlose);
    sendMessage(c2, winlose);
  }
  else{
    int msgLen = calculate_length("IT WAS A TIE!");
    char winlose[2+msgLen];
    memcpy(&winlose[2], "IT WAS A TIE!", msgLen);
    winlose[1] = msgLen;
    winlose[0] = 14;
    sendMessage(c1, winlose);
    sendMessage(c2, winlose);
  }
  // scores
  send_scores();

  // ASK NEW GAME
  msgLen = calculate_length("Do you want to play again? [1/0]");
  char new_game[2+msgLen];
  new_game[0] = 15;
  new_game[1] = msgLen;
  memcpy(&new_game[2], "Do you want to play again? [1/0]", msgLen);
  sendMessage(c1, new_game);
  sendMessage(c2, new_game);

  // ANSWER NEW GAME
  mail * in1 = create_mail();
  mail * in2 = create_mail();
  in1 = receiveMessage(c1);
  in2 = receiveMessage(c2);
  if(in1->msg == (char*)1 && in2->msg == (char*)1){
    play_game();
  }
  else{
    msgLen = calculate_length("No new game. Disconnecting.");
    char no_new_game[2+msgLen];
    no_new_game[0] = 15;
    no_new_game[1] = msgLen;
    memcpy(&no_new_game[2], "No new game. Disconnecting.", msgLen);
    sendMessage(c1, no_new_game);
    sendMessage(c2, no_new_game);
  }

}

void  INThandler(int sig){
  char  c;
  signal(sig, SIG_IGN);
  printf("OUCH, presionaste Ctrl-C?\nRealmente quieres salir? [y/n] ");
	c = getchar();
	if (c == 'y' || c == 'Y'){
    // Send disconnet_message();
    exit(0);
  }
}

int main (){
  signal(SIGINT, INThandler);
  create_log();
  initializeServer(IP, PORT);


  // PREPARE FOR GAME
  strcpy(nn1,set_up(c1));
  strcpy(nn2,set_up(c2));
  send_nicknames(c1, nn2);
  send_nicknames(c2, nn1);

  // PLAY GAME
  play_game();
  end_game();
  printf("LOG:\n%s\n", the_log->big_log);
  free(mi_juego);
  free(innbox_mail);
}
