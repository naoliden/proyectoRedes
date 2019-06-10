#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <signal.h>



typedef struct my_msg{
  char id;
  char length;
  char *msg;
} my_msg;

  my_msg * create_msg(){
    my_msg * new = malloc(sizeof(my_msg));
    new->msg = malloc(512*sizeof(char));
    return new;
  }
int initializeClient(char* ip, int port){
    int clientSocket;
    int mari = 1;
    //char buffer[1024];
    struct sockaddr_in serverAddr;
    socklen_t addr_size;

    /*---- Creación del Socket. Se pasan 3 argumentos ----*/
    /* 1) Internet domain 2) Stream socket 3) Default protocol (TCP en este caso) */
    clientSocket = socket(PF_INET, SOCK_STREAM, 0);
    setsockopt(clientSocket, SOL_SOCKET, SO_REUSEADDR, (char*)&mari, sizeof(mari));

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
my_msg * receiveMessage(int socket){
    char ID;
    recv(socket, &ID, 1, 0);
    char payloadSize;
    recv(socket, &payloadSize, 1, 0);
    char * message = malloc(payloadSize);
    recv(socket, message, payloadSize, 0);

    my_msg * m = create_msg();
    m->id = ID;
    m->length = payloadSize;
    strcpy(m->msg, message);
    return m;
}


void recieve_image(int socket){

  unsigned char packet_size = 128;
  int bufferSize = packet_size + 4;
	FILE * f = fopen("new_photo.jpg", "r+b");
	char ID;
	char total_packets;
	char current_payload;

	while(1){
      char payload_size = 0;
			char * photo_buffer = calloc(bufferSize, sizeof(char));

			recv(socket, photo_buffer, bufferSize, 0);

      memcpy(&ID, &photo_buffer[0], 1);
			memcpy(&total_packets, &photo_buffer[1], 1);
			memcpy(&current_payload, &photo_buffer[2], 1);

			memcpy(&payload_size, &photo_buffer[3], 1);

			char * writer_buffer = calloc(payload_size, sizeof(char));
			memcpy(writer_buffer, &photo_buffer[4], payload_size);

			fseek(f, current_payload * payload_size, SEEK_SET);
			fwrite(writer_buffer, packet_size, 1, f);

			free(writer_buffer);
			free(photo_buffer);

			if (current_payload == total_packets){
				break;
			}

	}

	fclose(f);
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
void setup(int socket){
//SEND SETUP MESSAGE
char package_connect[1];
package_connect[0] = 1;
sendMessage(socket,package_connect);

my_msg * conection_msg = create_msg();
conection_msg = receiveMessage(socket);
printf("%s\n", conection_msg->msg);

my_msg * whats_your_nickname = create_msg();
whats_your_nickname = receiveMessage(socket);
printf("%s \n", whats_your_nickname->msg);

//Client writes the nickname
char client_nickname[1024];
scanf("%[^\n]", client_nickname);
int nickname_ln = calculate_length(client_nickname);

//Create package to send
char pack_nickname[2+nickname_ln];
pack_nickname[0] = 4;
pack_nickname[1] = nickname_ln;
strcpy(&pack_nickname[2], client_nickname);
sendMessage(socket, pack_nickname);

//PRINT NICKENAME OF OPPONENT
my_msg * nickname_msg = create_msg();
nickname_msg = receiveMessage(socket);
printf("%s\n", nickname_msg->msg);


my_msg * game_is_starting = create_msg();
game_is_starting = receiveMessage(socket);
printf("%s\n", game_is_starting->msg);

//free(nickname_msg);
//free(whats_your_nickname);
//free(conection_msg);
//free(game_is_starting);
}
void chat_with_friends(int socket){
  //char * chat_message = calloc(255, sizeof(char));
  char * sentence = calloc(255,sizeof(char));

  printf("WRITE IN YOUR MESSAGE \n");

  char my_message[255];
	char temp;
	scanf("%c",&temp);
	scanf("%[^\n]",my_message);

  //scanf("[]%s", chat_message);
  memcpy(sentence, my_message, 255);
  int chat_length = calculate_length(sentence);

  char new_chat[2 + chat_length];
  new_chat[0] = 19;
  new_chat[1] = chat_length;

  strcpy(&new_chat[2], sentence);
  sendMessage(socket, new_chat);
  printf("YOUR MESSAGE HAS BEEN SENT. START PLAYING\n");

  //free(chat_message);
  //free(sentence);

  }
void do_not_want_to_chat(int socket){
  char chat_msg[4];
  chat_msg[0] = 19;
  chat_msg[1] = 0;
  sendMessage(socket, chat_msg);
  }
void receive_score(int socket){

  my_msg * score = create_msg();
  score = receiveMessage(socket);
  printf("%s \n", score->msg);
}

void see_the_board(int socket){
    my_msg * playboard = create_msg();
    playboard = receiveMessage(socket);
    int index = 0;
    printf("   | 1 | 2 | 3 | 4 | 5 | 6 | 7 | 8 |\n");
    printf("------------------------------------\n");

    for (int row=0; row<8; row++){
      printf(" %d |", row+1);
      for (int col=0; col<8; col++){
        printf(" %c |", playboard->msg[index]);
        index++;
      }
      printf("\n------------------------------------\n");
    }

    //free(playboard);

  }
void move_my_piece(int socket){

  int move = 1;

  while(move){
    int c_row, c_col, n_row, n_col;

    printf("Type in current ROW of the piece you want to move ");
    scanf("%d", &c_row);
    printf("\n");
    printf("Type in current COLOUMN of the piece you want to move ");
    scanf("%d", &c_col);
    printf("\n");
    printf("Type in new ROW of the piece you want to move ");
    scanf("%d", &n_row);
    printf("\n");
    printf("Type in new COLOUMN of the piece you want to move ");
    scanf("%d", &n_col);

    char my_move[6];
    my_move[0] = 10;
    my_move[1] = 6;

    my_move[2] = c_row;
    my_move[3] = c_col;
    my_move[4] = n_row;
    my_move[5] = n_col;

    // SENDING THE MESSAGE WITH MOVE TO SERVER
    sendMessage(socket, my_move);

    // WAITING FOR FEEDBACK
    my_msg * valid_move = create_msg();
    valid_move = receiveMessage(socket);

    if(valid_move->id == (unsigned char)11){
      printf("INVALID MOVE - TRY AGAIN \n");
      see_the_board(socket);
    }

    if(valid_move->id == (unsigned char)12){

      if(strcmp(valid_move->msg, "K")==0){
        printf("YOU KILLED SOMEONE! ITS YOUR TURN AGAIN. \n");
        see_the_board(socket);
        move_my_piece(socket);
      }

      else{
          printf("MOVED YOUR PIECE TO THE NEW POSITION. WELL DONE");
          move = 0;}
    }
  }
}


void view_score(int socket){
  my_msg * score = create_msg();
  score = receiveMessage(socket);
  printf(" %s \n", score->msg);
  //free(score);

}
void opponents_turn(int socket){
  view_score(socket);
}
void end_game(int socket){
  printf("The Game has ended\n");
  see_the_board(socket);
  my_msg * winner = create_msg();
  winner = receiveMessage(socket);
  printf("%s\n",winner->msg);
  //free(winner);

  view_score(socket);

  my_msg * want_to_play_more = create_msg();
  want_to_play_more = receiveMessage(socket);
  printf("%s\n", want_to_play_more->msg);
  //free(want_to_play_more);

  int input;
  scanf("%d", &input);

  while(input != 1 && input != 2){
    printf(" [Y = 1, N = 0]\n");
    scanf("%d", &input);}

  char play_more[4];
  play_more[0] = 16;
  play_more[1] = 0;
  play_more[2] = input;
  sendMessage(socket, play_more);
}

void my_turn(int socket){
  printf("It is your turn to play. \n");
  view_score(socket);

  my_msg * new_message = create_msg();
  new_message = receiveMessage(socket);

  if(new_message->id == 19){
    printf("ALERT NEW MESSAGE\n");
    printf("%s\n", new_message->msg);
    my_msg * my_options = create_msg();
    my_options = receiveMessage(socket);
    printf("%s\n", my_options->msg);
  }

    else {
      printf("%s\n", new_message->msg);
  }


  int my_option;
  scanf("%d", &my_option);
  while(my_option != 1 && my_option != 2 && my_option != 3){
    printf("Input either [1:Y] [2:N] or [3:exit]");
    scanf("%d", &my_option);
  }

  if(my_option == 1){
    chat_with_friends(socket);
    see_the_board(socket);
    move_my_piece(socket);
  }

else if (my_option == 2){
    do_not_want_to_chat(socket);
    see_the_board(socket);
    move_my_piece(socket);
  }

else if(my_option == 3){
  char end_connection[4];
  end_connection[0] = 17;
  end_connection[1] = 1;
  sendMessage(socket, end_connection);
  exit(0);

  }
}

int globalsocket;

void INThandler(int sig, int socket){

		 char  c;
		 signal(sig, SIG_IGN);
		 printf("OUCH, presionaste Ctrl-C?\nRealmente quieres salir? [y/n] ");
		 c = getchar();
     char end_connection_2[4];
		 if (c == 'y' || c == 'Y'){
       exit(0);

     }

		 }


int main(int argc, char *argv[]){
  char IP[64];
  int port;

  if (argc > 1){
    strcpy(IP, argv[2]);
    port = atoi(argv[4]);
  } else{
    strcpy(IP, "10.201.162.185");
    port = 8080;
  }

  printf("%s\n",IP);
  printf("%d\n", port);


    int clientSocket = initializeClient(IP, port);
    globalsocket=clientSocket;
    signal(SIGINT, INThandler);
    int exit = 1;
    setup(clientSocket);

    while(exit){
      my_msg * message = create_msg();
      message = receiveMessage(clientSocket);

      if(message->id == (unsigned char)13){
        end_game(clientSocket);}

      if(message->id == (unsigned char)8){
        int turn = atoi(message->msg);

        if(turn == 1){
          my_turn(clientSocket);
        }

          else if(turn == 2){
              opponents_turn(clientSocket);
            }
  }

      if(message->id == (unsigned char)17){
        printf("Connection lost. Game ended");
        exit = 0;
      }}
}
