// IP Anne
// #define IP "146.155.158.222"
#define IP "0.0.0.0"

// IP Norman
//#define IP "146.155.158.10"
#define PORT 8090

typedef struct Piece{
	int coordenadas[2];
	int player;
	int dama;
} Piece;

typedef struct Board{
	unsigned char board[(8 + 7 * 3) ^ 2];
	Piece piezas[24];
} Board;

typedef struct Message{
	char id;
	char payload;
	int size;
} Message;


char* recieveMessage(int socket, char* message);

void sendMessage(int socket, char* message);

static void sig_handler(int _);

/* Intenta conectarse con el servidor, si no puede imprime que no puede. */
int initializeClient(char* ip, int port);

/* Envia un mensaje al servidor con la posicion a la que se quiere mover la pieza */
void move();

/* Envia una mensaje al servidor */
void send_message(int clientSocket, char * mensaje);

/* Crea un mensaje de chat para enviar */
void chat_message();

/* Espera por el siguiente mensaje del servidor */
void escuchar(int clientSocket);

/* Muestra el tablero */
void show_board();

/* Envia un mensaje de fin del turno */
void end_turn();

/* Envia mensaje de desconexion */
void disconnect();



