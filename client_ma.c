#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>

#define PORT 4455


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

int compare_strings(char a[], char b[])
{
    int c = 0;
    while (a[c] == b[c])
    {
        if (a[c] == '\0' || b[c] == '\0')
        break;
        c++;
    }
    if (a[c] == '\0' && b[c] == '\0')
    return 0;
    else
    return -1;
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
char client_nickname[100];
scanf("%s", client_nickname);
int nickname_ln = calculate_length(client_nickname);

//Create package to send
char pack_nickname[2+nickname_ln];
pack_nickname[0] = 4;
pack_nickname[1] = nickname_ln;
strcpy(&pack_nickname[2], client_nickname);
print_package(pack_nickname);
sendMessage(socket, pack_nickname);

//PRINT NICKENAME OF OPPONENT
my_msg * nickname_msg = create_msg();
nickname_msg = receiveMessage(socket);
printf("%s\n", nickname_msg->msg);

my_msg * game_is_starting = create_msg();
game_is_starting = receiveMessage(socket);
printf("%s\n", game_is_starting->msg);

// who is first

my_msg * who_is_first = create_msg();
who_is_first = receiveMessage(socket);

if((int) who_is_first->msg == 1){
  printf("YOU START THE GAME \n");
}
else{
  printf("THE OPPONENT START THE GAME \n");
}


free(nickname_msg);
free(whats_your_nickname);
free(conection_msg);
free(game_is_starting);
free(who_is_first);

}

void chat_with_friends(int socket){
  printf("WRITE IN YOUR MESSAGE \n");
  char buffer[1024];
  scanf("%s", buffer);
  int chat_length = calculate_length(buffer);
  char new_chat[2 + chat_length];
  new_chat[0] = 19;
  new_chat[1] = chat_length;
  strcpy(&new_chat[2], buffer);
  sendMessage(socket, new_chat);
  }

void see_the_board(int socket){

    char see_board_message[2];
    see_board_message[0] = 9;
    see_board_message[1] = 2;
    sendMessage(socket, see_board_message);

    my_msg * playboard = create_msg();
    playboard = receiveMessage(socket);
    printf("%s \n", playboard->msg);

    int index = 0;
    printf("   | 0 | 1 | 2 | 3 | 4 | 5 | 6 | 7 |\n");
    printf("------------------------------------\n");

    for (int row=0; row<8; row++){
      printf(" %d |", row);
      for (int col=0; col<8; col++){
        printf(" %c |", playboard->msg[index]);
        index++;
      }
      printf("\n------------------------------------\n");
    }

  }

void move_my_piece(int socket){
  int move = 1;

  while(move){
    int c_row, c_col, n_row, n_col;
    printf("Type the location of the piece you want to move");
    scanf("%d, %d", &c_row, &c_col);
    printf("\n");

    printf("Type in the location you want to move to");
    scanf("%d, %d", &n_row, &n_col);

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
      printf("INVALID MOVE - TRY AGAIN");
    }

    else if(valid_move->id ==  (unsigned char)12){
      printf("MOVED YOUR PIECE TO THE NEW POSITION. WELL DONE");
      move = 0;
      free(valid_move);}
    }

  }

void view_score(int socket){
  my_msg * score = create_msg();
  score = receiveMessage(socket);
  printf("The score is %s: \n", score->msg);

}

int main(){
    int exit = 1;
    int clientSocket = initializeClient("10.201.149.195", PORT);
    // SAYING HI TO THE SERVER, SENDING NICKNAME, GETTING THE OPPONENT ETC
    setup(clientSocket);

    while(exit){
      //RECEIEVE OPTIONS FROM SERVER
      my_msg * new_message = create_msg();
      new_message = receiveMessage(clientSocket);

      // IS THE INBOX FULL OR NOT
      if(new_message->id == 19){
        printf("ALERT NEW MESSAGE: WANT TO RESPOND TYPE 1: \n");
        printf("%s\n", new_message->msg);
        my_msg * my_options = create_msg();
        my_options = receiveMessage(clientSocket);
        printf("%s\n", my_options->msg);
        free(my_options);}

      // RECEIEVE OPTIONS IF THE LAST MSG WAS A CHAT MSG
      else{
        printf("%s\n", new_message->msg);}

      // ASKING INPUT FROM THE USER
      int my_option;
      printf("TYPE IN YOUR OPTION?");
      scanf("%d", &my_option);


      if(my_option == 1){
        chat_with_friends(clientSocket);
      }


      else if (my_option == 2){
        see_the_board(clientSocket);
      }


      else if(my_option == 3){
        view_score(clientSocket);

      }

      else if(my_option == 4){
        see_the_board(clientSocket);
        move_my_piece(clientSocket);

      }
      else if(my_option == 5){
        printf("EXITING THE GAME");
        exit = 0;
      }

      else{
        printf("INPUT IS EITHER 1, 2, 3, 4 OR 5 YOU STUPID!! HOW MANY TIMES MUST I TELL YOU.");
      }
}}
