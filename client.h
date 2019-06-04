#define IP "0.0.0.0"
#define PORT 8080


typedef struct Pieces{
	int coordenadas[2];
	int player;
	int dama;

} Pieces;

typedef struct Board{
	unsigned char board[(8 + 7 * 3) ^ 2];
	Pieces piezas[24];
};

typedef struct Message{
	char id;
	char payload;
	int size;

} Message;


/* Intenta conectarse con el servidor, si no puede imprime que no puede. */
int * initializeClient(char* ip, int port, int * clientSocket);

/* Envia un mensaje al servidor con la posicion a la que se quiere mover la pieza */
void move();

/* Envia una mensaje al servidor */
void send_message();

/* Crea un mensaje de chat para enviar */
void chat_message();

/* Espera por el siguiente mensaje del servidor */
void listen();

/* Muestra el tablero */
void show_board();

/* Envia un mensaje de fin del turno */
void end_turn();

/* Envia mensaje de desconexion */
void disconnect();

void intHandler(int _);



