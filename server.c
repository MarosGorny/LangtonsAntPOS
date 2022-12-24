#include "client_server_definitions.h"

#include <stdlib.h>
#include <string.h>

#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <pthread.h>
#include <stdio.h>


typedef struct sockaddr_in SA_IN;
typedef struct sockaddr SA;
#define BUFSIZE 4096
#define SERVER_BACKLOG 100

int main(int argc, char* argv[]) {
    if (argc < 3) {
        printError("Sever je nutne spustit s nasledujucimi argumentmi: port pouzivatel.");
    }
    int port = atoi(argv[1]);
    if (port <= 0) {
        printError("Port musi byt cele cislo vacsie ako 0.");
    }
    char *userName = argv[2];

    int serverSocket,clientSocket,addr_size;
    SA_IN  server_addr, client_addr;

    //vytvorenie TCP socketu <sys/socket.h>
    serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket < 0) {
        printError("Chyba - socket.");
    }

    //definovanie adresy servera <arpa/inet.h>
    server_addr.sin_family = AF_INET;         //internetove sockety
    server_addr.sin_addr.s_addr = INADDR_ANY; //prijimame spojenia z celeho internetu
    server_addr.sin_port = htons(port);       //nastavenie portu

    //prepojenie adresy servera so socketom <sys/socket.h>
    if (bind(serverSocket, (SA *)&server_addr, sizeof(server_addr)) < 0) {
        printError("Chyba - bind.");
    }

    //server bude prijimat nove spojenia cez socket serverSocket <sys/socket.h>
    if(listen(serverSocket, SERVER_BACKLOG) < 0) {
        printError("Chyba - listen.");
    }

    while (1) {
        printf("Waiting for connections..\n");
        //wait for, and eventually accept an incoming connection
        addr_size = sizeof(SA_IN);
        if((clientSocket = accept(serverSocket, (SA *)&client_addr, (socklen_t*)&addr_size)) < 0) {
            printError("Chyba - accept");
        }
        printf("Connected!\n");

        //uzavretie pasivneho socketu <unistd.h>
        close(serverSocket);

        //inicializacia dat zdielanych medzi vlaknami
        DATA data;
        data_init(&data, userName, clientSocket);

        //vytvorenie vlakna pre zapisovanie dat do socketu <pthread.h>
        pthread_t thread;
        pthread_create(&thread, NULL, data_writeData, (void *)&data);

        //v hlavnom vlakne sa bude vykonavat citanie dat zo socketu
        data_readData((void *)&data);

        //pockame na skoncenie zapisovacieho vlakna <pthread.h>
        pthread_join(thread, NULL);
        data_destroy(&data);

        //uzavretie socketu klienta <unistd.h>
        close(clientSocket);

        break;
    }
    return (EXIT_SUCCESS);
}

void hande_connection(int client_socket) {

}