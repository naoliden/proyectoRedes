#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>

#define PORT 8080

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

char* receiveMessage(int socket){
    printf("Waiting message... \n");
    // Esperamos a que llegue el primer byte, que corresponde al ID del paquete
    char ID;
    recv(socket, &ID, 1, 0);
    printf("\n####### Paquete recibido ####\n");
    printf("The ID is: %d\n", ID); // lo imprimimos como número porque esa es la interpretación que acordamos
    
    // Recibimos el payload size en el siguiente byte
    char payloadSize;
    recv(socket, &payloadSize, 1, 0);
    printf("The PayloadSize is: %d\n", payloadSize);
    
    // Recibimos el resto del paquete, según el payloadSize. Lo guardamos en un puntero de caracteres, porque no es necesario modificarlo
    char * message = malloc(payloadSize);
    recv(socket, message, payloadSize, 0);
    printf("The Message is: %s\n", message);
    printf("#############################\n");
    
    // Aqui se las ingenian para ver como retornan todo. Puden retornar el paquete y separarlo afuera, o retornar una struct.
    return message;
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

//RECEIVE MESSAGE FOR CONNECTION
char * con_msg = receiveMessage(socket);
printf("%s\n", con_msg);

//Receive request for nickname from server
char * whats_your_nickname = receiveMessage(socket);
printf("%s \n", whats_your_nickname);

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
char *nickname_msg = receiveMessage(socket);
printf("%s\n", nickname_msg);
}


int main(){
    int clientSocket = initializeClient("10.201.149.195", PORT);
    setup(clientSocket);
    char *msg_2 = receiveMessage(clientSocket);
    
    printf("Message from server: %s\n", msg);
    
    
}
