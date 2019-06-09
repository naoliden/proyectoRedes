
// IP Anne
// #define IP "146.155.158.222"
// IP Norman
//#define IP "146.155.158.10"#define PORT 8080

#define IP "10.201.149.195"
#define PORT 4455


typedef struct Client{
	int clientSocket;
	int player;
	int connected;
	char player_name[27];
} Client;

typedef struct Conexiones {
	Client p1;
	Client p2;
} Conexiones;

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


int initializeServer(char* ip, int port);

static void sig_handler(int _);

void server_init(char* ip, int port, int* welcomeSocket, Client* clientOne, Client* clientTwo);

char* recieveMessage(int socket, char* message);

void sendMessage(int socket, char* message);
