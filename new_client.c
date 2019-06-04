#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>

//fonction pour comparer deux strings : renvoie 0 si les valeurs sont égales
//et -1 sinon

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


int main() {
    //déclaration des variables
    int clientSocket;
    char buffer[1024];
    struct sockaddr_in serverAddr;
    socklen_t addr_size;
    int cmdEXIT = 0;
    
    //paramètrage du socket
    clientSocket = socket(PF_INET, SOCK_STREAM, 0);
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(7891);
    serverAddr.sin_addr.s_addr = inet_addr("10.201.145.158");
    memset(serverAddr.sin_zero, '\0', sizeof serverAddr.sin_zero);
    addr_size = sizeof serverAddr;
    
    //connection au serveur
    connect(clientSocket, (struct sockaddr *) &serverAddr, addr_size);
    
    //premier message du Client1
    recv(clientSocket, buffer, 1024, 0);
    scanf(" %[^\n]s", buffer);
    send(clientSocket,buffer,sizeof buffer - 1,0);
    
    //continuer à envoyer et recevoir des messages
    //tant qu'un des clients n'envoive pas "exit"
    while (cmdEXIT == 0)
    {
        //si le message envoyé n'est pas "exit"
        if (compare_strings(buffer, "exit")==-1)
        {
            //vider le buffer
            memset(&buffer[0], 0, sizeof(buffer));
            //la valeur de recv qui est égale a 1 si recv n'a pas
            //encore reçu de message
            //sinon, elle est égale au nombre de bits reçu
            int recvValue = recv(clientSocket, buffer, sizeof buffer - 1, 0);
            //si recv n'est pas égal a 1 => un message a été reçu
            if (recvValue != 1)
            {
                //si le contenu n'est pas "exit"
                if (compare_strings(buffer, "exit")==-1)
                {
                    //afficher le message du Client2
                    printf("Client 2 : ");
                    printf("%s\n", buffer);
                    //vider le buffer
                    memset(&buffer[0], 0, sizeof(buffer));
                }
                //si Client2 a envoyé "exit"
                else cmdEXIT=1;
            }
            //si rcv est égal a 1 => pas de message reçu
            else
            {
                //Client1 peut saisir son message
                printf("Client 1 : ");
                scanf(" %[^\n]s", buffer);
                //et l'envoyer à Client2
                send(clientSocket,buffer,sizeof buffer - 1,0);
            }
        }
        //sinon finir la boucle
        else cmdEXIT=1;
    }
    
    return 0;
}
