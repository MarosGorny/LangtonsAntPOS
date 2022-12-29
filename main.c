#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>
#include <string.h>

#include "ant.h"
#include "display.h"
#include "settings.h"


#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include "structuresEnums.h"
#include "server_definition.h"


void* serverRead(void* data) {
    printLog("void* serverRead(void* data)");
    DATA *pdata = (DATA *)data;
}


void* clientsAcceptF(void* data) {
    printLog("void* clientsAcceptF(void* data)");
    DATA *pdata = (DATA *)data;
    int clientSocket;
    int serverSocket = pdata->acceptDataForServer->serverSocket;
    SA_IN* client_addr = pdata->acceptDataForServer->client_addr;
    int* addr_size = pdata->acceptDataForServer->addr_size;

    while(true) {
        int numberOfClients = pdata->numberOfClients;

        if((clientSocket = accept(serverSocket, (SA *)&client_addr, (socklen_t*)&addr_size)) < 0) {
            printError("Error - accept");
        } else {
            pdata->sockets[numberOfClients] = clientSocket;
            pdata->numberOfClients++;
            printf("Client[%d] connected!\n",numberOfClients);
        }
        pthread_mutex_lock(&pdata->mutex);
        if(pdata->numberOfClients > 0) {
            pthread_cond_signal(pdata->condStartListeningArray);
        }
        pthread_mutex_unlock(&pdata->mutex);
    }
}

void* antSimulation(void* data) {
    // split into frames from wiki
    // https://ezgif.com/split/ezgif-2-6771e98488.gif

//    FILE *fptrRead;
//

    bool repeat = true;

    printLog("void* antSimulation(void* data)");

    while (repeat) {

        DATA *pdata = (DATA *)data;

        int rows;
        int columns;
        int numberOfAnts;
        LOGIC_TYPE logicType;
        LOADING_TYPE loadingType;


        pthread_mutex_lock(&pdata->mutex);
        printLog("Before: pthread_cond_wait(&pdata->startAntSimulation,&pdata->mutex)");
        pthread_cond_wait(&pdata->startAntSimulation, &pdata->mutex);
        printLog("After: pthread_cond_wait(&pdata->startAntSimulation,&pdata->mutex)");
        rows = pdata->rows;
        columns = pdata->columns;
        numberOfAnts = pdata->numberOfAnts;
        logicType = pdata->logicType;
        loadingType = pdata->loadingType;
        pthread_mutex_unlock(&pdata->mutex);
        printf("After taking data from pdata game\n");

    //    if(loadingType == FILE_INPUT ) {
    //        getNumberRowsCollumns(fptrRead,&rows,&columns);
    //    }
        printf("WIDTH:%d HEIGHT:%d\n", columns, rows);

        pthread_mutex_t mainMut = PTHREAD_MUTEX_INITIALIZER;
        //Creating barrier
        pthread_barrier_t barriers[numberOfAnts];
        for (int i = 0; i < numberOfAnts; i++) {
            pthread_barrier_init(&barriers[i], NULL, i+1);
        }
        printf("Barrier\n");
        //Creating display
        DISPLAY display = {columns, rows, numberOfAnts, barriers,&barriers[numberOfAnts-1], &mainMut, logicType, NULL,pdata};
        //Creating 2D dynamic array of boxes , pointer of pointers
        display.box = malloc(rows*sizeof (BOX**));
        //Creating mutexes
        pthread_mutex_t boxMutexes[rows][columns];

        //Randomness of black boxes
        int chanceOfBlackBox;
        if (loadingType == RANDOM_COLOR) {
            chanceOfBlackBox = getChanceOfBlackBox();
        }

    //    if(loadingType == FILE_INPUT) {
    //        int temp;
    //        fptrRead = fopen("../txtFiles/test.txt","r");
    //        fscanf(fptrRead,"%d", &temp);
    //        fscanf(fptrRead,"%d", &temp);
    //    }

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
    //                    initBoxFile(boxData,fptrRead);
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
    //    if(loadingType == FILE_INPUT) {
    //        fclose(fptrRead);
    //    }
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
        int *dataStop = malloc(sizeof (int));
        *dataStop = 0;
        //printf("1111111111111111111\n");
        for (int i = 0; i < numberOfAnts; i++) {
            //printf("22222222222222222\n");
            pthread_join(ants[i],&counter);
            if(counter != NULL) {
                counterOfFinishedAnts += 1;
                if(*(int*)(counter) == -1) {
                    *(int*)(dataStop) = 1;
                }
                //counterOfFinishedAnts += *((int*)counter);
                free(counter);
            }

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
        printf("Writing to file\n");
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
        printf("Cleaning and deleting\n");
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


        pthread_mutex_destroy(&mainMut);
        //Destroying barrier
        for (int i = 0; i < numberOfAnts; i++) {
            pthread_barrier_destroy(&barriers[i]);
        }
        if (*dataStop == 1) {
            repeat = false;
        }
        free(dataStop);
        printf("END \n");
    }
    return EXIT_SUCCESS;
}

int main(int argc,char* argv[]) {

    ////SERVER
    printLog("main: server");
    if (argc < 3) {
        printError("Sever have to be launched with following arguments: port username.");
    }
    int port = atoi(argv[1]);
    if (port <= 0) {
        printError("Port have to be integer greater than 0.");
    }
    char *userName = argv[2];

    int serverSocket, clientSocket, clientSocket2, addr_size;
    SA_IN server_addr, client_addr;

    //vytvorenie TCP socketu <sys/socket.h>
    serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket < 0) {
        printError("Error - socket.");
    }

    //definovanie adresy servera <arpa/inet.h>
    server_addr.sin_family = AF_INET;         //internetove sockety
    server_addr.sin_addr.s_addr = INADDR_ANY; //prijimame spojenia z celeho internetu
    server_addr.sin_port = htons(port);       //nastavenie portu

    //prepojenie adresy servera so socketom <sys/socket.h>
    if (bind(serverSocket, (SA *) &server_addr, sizeof(server_addr)) < 0) {
        printError("Error - bind.");
    }

    //server bude prijimat nove spojenia cez socket serverSocket <sys/socket.h>
    if (listen(serverSocket, SERVER_BACKLOG) < 0) {
        printError("Error - listen.");
    }

    printf("Waiting for connections..\n");

    //wait for, and eventually accept an incoming connection
    addr_size = sizeof(SA_IN);


    //inicializacia dat zdielanych medzi vlaknami
    DATA data;
    data_init(&data, userName);

    pthread_cond_t startListening[SERVER_BACKLOG];
    data.condStartListeningArray = startListening;

    data.sockets = (int *) calloc(SERVER_BACKLOG, sizeof(int));


    data.numberOfClients = 0;
    data.step = 1;







    //inicializacia vlakna na ktorom bude prebiehat simulacia
    pthread_t gameThread;
    pthread_create(&gameThread, NULL, antSimulation, (void *) &data);

    //vytvorenie vlakna pre zapisovanie dat do socketu <pthread.h>
    pthread_t threadWrite[SERVER_BACKLOG];

    //vytvorenie vlakna  na citanie dat zo socketu
    pthread_t threadRead[SERVER_BACKLOG];

    //v hlavnom vlakne sa budu prijimat novi clienti
    while (true) {
        int numberOfClients = data.numberOfClients;

        if ((clientSocket = accept(serverSocket, (SA *) &client_addr, (socklen_t *) &addr_size)) < 0) {
            printError("Error - accept");
        } else {
            data.sockets[numberOfClients] = clientSocket;
            data.numberOfClients++;
            writeStateOfSharedData(&data,data.sockets[numberOfClients]);
            pthread_create(&threadWrite[numberOfClients], NULL, data_writeData, (void *) &data);
            pthread_create(&threadRead[numberOfClients], NULL, data_readData, (void *) &data);
            pthread_cond_init(&data.condStartListeningArray[numberOfClients], NULL);

            printf("Client[%d] connected = socket%d !\n", numberOfClients,clientSocket);

            if(data.stop == 1) {
                break;
            }
        }
    }



        //pockame na skoncenie zapisovacieho vlakna <pthread.h> a vlakna na simulacia mravcov
    pthread_join(gameThread, NULL);

    for (int i = 0; i < SERVER_BACKLOG; i++) {
        pthread_join(threadWrite[i], NULL);
    }
    for (int i = 0; i < SERVER_BACKLOG; i++) {
        pthread_join(threadRead[i], NULL);
    }



    free(data.sockets);
    data_destroy(&data);

    //uzavretie socketu klienta <unistd.h>
    close(clientSocket);
    //uzavretie pasivneho socketu <unistd.h>
    //TODO ZMAZAL SOM KVOLI TUTORIALU
    close(serverSocket);


    return (EXIT_SUCCESS);

    ////END-SERVER
    //////////////////////////////////////////
}
