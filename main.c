#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>
#include <string.h>

#include "ant.h"
#include "display.h"
#include "settings.h"
#include "client_server_definitions.h"
//#include "client_server_definitions.h"


#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>

typedef struct sockaddr_in SA_IN;
typedef struct sockaddr SA;
#define BUFSIZE 4096
#define SERVER_BACKLOG 100

typedef struct initSettings{

}ANTS_INIT_SETTINGS;

int main(int argc,char* argv[]) {
    // split into frames from wiki
    // https://ezgif.com/split/ezgif-2-6771e98488.gif

    int columns;
    int rows;
    int numberOfAnts;
    FILE *fptrRead;

    LOADING_TYPE loadingType;
    LOGIC_TYPE logicType;

    columns = rows = 5;
    //////////////////////////////////////////
    ////SERVER
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


    ////END-SERVER
    //////////////////////////////////////////


    loadingType = setLoadingType();
    logicType = setLogicType();
    numberOfAnts = setNumberOfAnts();

    if(loadingType == FILE_INPUT ) {
        getNumberRowsCollumns(fptrRead,&rows,&columns);
    }
    printf("WIDTH:%d HEIGHT:%d\n", columns, rows);

    pthread_mutex_t mainMut = PTHREAD_MUTEX_INITIALIZER;
    //Creating barrier
    pthread_barrier_t barriers[numberOfAnts];
    for (int i = 0; i < numberOfAnts; i++) {
        pthread_barrier_init(&barriers[i], NULL, i+1);
    }
    printf("Barrier\n");
    //Creating display
    DISPLAY display = {columns, rows, numberOfAnts, barriers,&barriers[numberOfAnts-1], &mainMut, logicType};
    //Creating 2D dynamic array of boxes , pointer of pointers
    display.box = malloc(rows*sizeof (BOX**));
    //Creating mutexes
    pthread_mutex_t boxMutexes[rows][columns];

    //Randomness of black boxes
    int chanceOfBlackBox;
    if (loadingType == RANDOM_COLOR) {
        chanceOfBlackBox = getChanceOfBlackBox();
    }

    if(loadingType == FILE_INPUT) {
        int temp;
        fptrRead = fopen("../txtFiles/test.txt","r");
        fscanf(fptrRead,"%d", &temp);
        fscanf(fptrRead,"%d", &temp);
    }

    //Initialization of boxes and creating

    for (int i = 0; i < rows; i++) {
        //Creating ROWS of boxes
        display.box[i] = malloc(columns*sizeof (BOX*));
        for (int j = 0; j < columns; j++) {
            //***BOXES***
            //Creating box
            BOX* boxData = malloc(sizeof (BOX));
            //Assigning box to position [i][j]
            display.box[i][j] = boxData;
            //Initialization of box
            boxData->x = j;
            boxData->y = i;
            switch (loadingType) {
                case ALL_WHITE:
                case TERMINAL_INPUT:
                    boxData->color = WHITE;
                    break;
                case RANDOM_COLOR:
                    initBoxRandom(boxData,chanceOfBlackBox);
                    break;
                case FILE_INPUT:
                    initBoxFile(boxData,fptrRead);
                    break;
                default:
                    //TODO WHAT TO PRINT?
                    break;
            }
            //Mutex initialization and assignation to boxData
            boxMutexes[i][j] = (pthread_mutex_t)PTHREAD_MUTEX_INITIALIZER;
            boxData->mut = &boxMutexes[i][j];
        }
    }
    if(loadingType == FILE_INPUT) {
        fclose(fptrRead);
    }
    if (loadingType == TERMINAL_INPUT) {
       initBoxTerminalInput(&display);
    }

    //pthreads of ants
    pthread_t ants[numberOfAnts];
    //data of ants
    ANT antsD[numberOfAnts];
    //Prints background
    printBackground((const BOX ***) display.box, rows, columns);

    //TODO PUTING ANTS TO DISPLAY
    for (int i = 0; i < numberOfAnts; i++) {
        antsD[i].id = i+1;
        chooseAntsPosition(rows,columns,&antsD[i]);
        antsD[i].display = &display;
        pthread_create(&ants[i],NULL,antF,&antsD[i]);
        printf("Created ant[%d]\n",i+1);
    }

    void *counter = 0;
    int counterOfFinishedAnts = 0;
    //printf("1111111111111111111\n");
    for (int i = 0; i < numberOfAnts; i++) {
        //printf("22222222222222222\n");
        pthread_join(ants[i],&counter);
        counterOfFinishedAnts += *((int*)counter);
        free(counter);
    }
    //printf("333333333333333333333333333333333\n");
    printf("FINISHED ANTS %d\n",counterOfFinishedAnts);
    //Prints background
    printBackground((const BOX ***) display.box, rows, columns);
    //time and filename
    time_t t = time(NULL);
    struct tm tm = *localtime(&t);
    char fileNameString[50];
    sprintf(fileNameString, "../txtFiles/writedFile_%02dhod_%02dmin.txt", tm.tm_hour, tm.tm_min);
    //Writing to file
    FILE *fptr;
    fptr = fopen(fileNameString,"w");
    if(fptr == NULL) {
        printf("Error writing\n");
    } else {
        fprintf(fptr,"%d\n",rows);
        fprintf(fptr,"%d\n",columns);
        for (int i = 0; i < rows; i++) {
            for (int j = 0; j < columns; j++) {
                //Get tempBox
                if (display.box[i][j]->color == WHITE) {
                    fprintf(fptr,"0");
                } else if (display.box[i][j]->color == BLACK) {
                    fprintf(fptr,"1");
                } else {
                    fprintf(fptr,"X");
                };
            }
            fprintf(fptr,"\n");
        }
    }
    fclose(fptr);
    /// CLEANING AND DELETING
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < columns; j++) {
            //Get tempBox
            BOX* tempBox = display.box[i][j];
            //Destroy mutex
            pthread_mutex_destroy(tempBox->mut);
            //Destroy box
            free(tempBox);
        }
        //Destroy rows
        free(display.box[i]);
    }
    //Destroy whole 2Darray
    free(display.box);
    //pthread_barrier_destroy(barriers);
    pthread_mutex_destroy(&mainMut);
    //Destroying barrier
    for (int i = 0; i < numberOfAnts; i++) {
        pthread_barrier_destroy(&barriers[i]);
    }
    return 0;
}