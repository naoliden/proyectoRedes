//
//  new_server.c
//  
//
//  Created by Mari Tuhus on 04/06/2019.
//

#include "new_server.h"
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
    //déclaration des variables : Serveur et deux Clients
    int welcomeSocket, Client1, Client2;
    struct sockaddr_in serverAddr;
    struct sockaddr_storage serverStorage;
    socklen_t addr_size;
    char buffer[1024];
    
    //paramètrage du Serveur
    welcomeSocket = socket(PF_INET, SOCK_STREAM, 0);
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(7891);
    serverAddr.sin_addr.s_addr = inet_addr("127.0.01");
    memset(serverAddr.sin_zero, '\0', sizeof serverAddr.sin_zero);
    bind(welcomeSocket, (struct sockaddr *) &serverAddr, sizeof(serverAddr));
    
    //Serveur à l'écoute
    if (listen(welcomeSocket,5)==0)
        printf("Listening\n");
    else
        printf("Error\n");
    
    //lier le serveur et les deux clients
    addr_size = sizeof serverStorage;
    Client1 = accept(welcomeSocket, (struct sockaddr *) &serverStorage, &addr_size);
    Client2 = accept(welcomeSocket, (struct sockaddr *) &serverStorage, &addr_size);
    
    int cmdEXIT = 0;
    //continuer à recevoir et envoyer des messages
    //tant qu'un des clients n'envoive pas "exit"
    while (cmdEXIT == 0)
    {
        //recevoir le message de Client1
        recv(Client1, buffer, 1024, 0);
        //le renvoyer a Client2
        printf ("%s\nEnvoie au Client2\n", buffer);
        send(Client2,buffer,1024,0);
        //sortir de la boucle si Client1 a envoyé "exit"
        if (compare_strings(buffer, "exit")==0)
        {
            cmdEXIT = 1;
        }
        //sinon
        else
        {
            //vider le buffer
            memset(&buffer[0], 0, sizeof(buffer));
            //recevoir le message de Client2
            recv(Client2, buffer, 1024, 0);
            //le renvoyer a Client1
            printf ("%s\nEnvoie au Client1\n", buffer);
            send(Client1,buffer,1024,0);
            //si Client2 a envoyé "exit"
            if (compare_strings(buffer, "exit")==0)
            {
                cmdEXIT = 1;
            }
        }
    }
    
    return 0;
}
