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
#include "client_server_definitions.h"


//void* serverRead(void* data) {
//    printLog("void* serverRead(void* data)");
//    DATA *pdata = (DATA *)data;
//}


//void* clientsAcceptF(void* data) {
//    printLog("void* clientsAcceptF(void* data)");
//    DATA *pdata = (DATA *)data;
//    int clientSocket;
//    int serverSocket = pdata->acceptDataForServer->serverSocket;
//    SA_IN* client_addr = pdata->acceptDataForServer->client_addr;
//    int* addr_size = pdata->acceptDataForServer->addr_size;
//
//    while(true) {
//        int numberOfConnections = pdata->numberOfConnections;
//
//        if((clientSocket = accept(serverSocket, (SA *)&client_addr, (socklen_t*)&addr_size)) < 0) {
//            printError("Error - accept");
//        } else {
//            pdata->sockets[numberOfConnections] = clientSocket;
//            pdata->numberOfConnections++;
//            printf("Client[%d] connected!\n",numberOfConnections);
//        }
//        pthread_mutex_lock(&pdata->mutex);
//        if(pdata->numberOfConnections > 0) {
//            pthread_cond_signal(pdata->condStartListeningArray);
//        }
//        pthread_mutex_unlock(&pdata->mutex);
//    }
//}


/**
* Hlavne vlakno pre simulaciu. Zbieraju sa tu zadane nastavenia a inicializuje sa cela simulacia.
* V tomto vlakne sa tiez inicializuju vlakna pre kazdeho mravca a nasledne sa spusta simulacia.
*
* @param  data void pointer na DATA*
* @return void void pointer
*/
void* antSimulation(void* data) {
    // split into frames from wiki
    // https://ezgif.com/split/ezgif-2-6771e98488.gif

    FILE *fptrRead;
//

    bool repeat = true;

    printLog("void* antSimulation(void* data)");

    while (repeat) {
        printLog("while (repeat)");
        DATA *pdata = (DATA *)data;

        int rows;
        int columns;
        int numberOfAnts;
        LOGIC_TYPE logicType;
        LOADING_TYPE loadingType;
        char* textFileName;
        char fullPath[100];
        int typeOfCollision;


        pthread_mutex_lock(&pdata->mutex);
        printLog("Before: pthread_cond_wait(&pdata->startAntSimulation,&pdata->mutex)");
        pthread_cond_wait(&pdata->startAntSimulation, &pdata->mutex);
        if(pdata->stop == 1) {
            printf("GAME THREAD SHUTED DOWN\n");
            break;
        }
        printLog("After: pthread_cond_wait(&pdata->startAntSimulation,&pdata->mutex)");
        rows = pdata->rows;
        columns = pdata->columns;
        numberOfAnts = pdata->numberOfAnts;
        logicType = pdata->logicType;
        loadingType = pdata->loadingType;
        textFileName = pdata->txtFileName;
        typeOfCollision = pdata->typeOfCollision;
        pthread_mutex_unlock(&pdata->mutex);

        if(loadingType == FILE_INPUT_LOCAL) {
            //tu sa najprv vytvori na temp.txt
            sprintf(fullPath,"%s/temp.txt",getPWD());

            int temp;
            fptrRead = fopen(fullPath,"r");
            fscanf(fptrRead,"%d", &temp);
            rows = temp;
            fscanf(fptrRead,"%d", &temp);
            columns = temp;
        } else if(loadingType == FILE_INPUT_SERVER) {

            sprintf(fullPath,"%s/%s",getPWD(),textFileName);

            int temp;
            fptrRead = fopen(fullPath,"r");
            fscanf(fptrRead,"%d", &temp);
            rows = temp;
            fscanf(fptrRead,"%d", &temp);
            columns = temp;
        }

        printf("After taking data from pdata game\n");

    //    if(loadingType == FILE_INPUT_LOCAL ) {
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
        DISPLAY display = {columns, rows, numberOfAnts, barriers,&barriers[numberOfAnts-1], &mainMut, logicType, NULL,pdata,typeOfCollision};
        //Creating 2D dynamic array of boxes , pointer of pointers
        display.box = malloc(rows*sizeof (BOX**));
        //Creating mutexes
        pthread_mutex_t boxMutexes[rows][columns];

        //Randomness of black boxes
        int chanceOfBlackBox;
        if (loadingType == RANDOM_COLOR) {
            chanceOfBlackBox = getChanceOfBlackBox();
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
                boxData->numberOfAnts = 0;
                switch (loadingType) {
                    case ALL_WHITE:
                    case TERMINAL_INPUT:
                        boxData->color = WHITE;
                        break;
                    case RANDOM_COLOR:
                        initBoxRandom(boxData,chanceOfBlackBox);
                        break;
                    case FILE_INPUT_LOCAL:
                        initBoxFile(boxData,fptrRead);
                        remove(fullPath);
                        break;
                    case FILE_INPUT_SERVER:
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
        if(loadingType == FILE_INPUT_LOCAL) {
            fclose(fptrRead);
            //remove("/home/gorny/temp.txt");
        }
        if (loadingType == TERMINAL_INPUT) {
            for (int i = 0; i < rows; i++) {
                for (int j = 0; j < columns; j++) {
                    display.box[i][j]->color = display.dataSocket->colorOfDisplay[i][j];
                }
            }
            //initBoxTerminalInput(&display);
        }

        //pthreads of ants
        pthread_t ants[numberOfAnts];
        //data of ants
        ANT antsD[numberOfAnts];
        //Prints background
        printBackground((const BOX ***) display.box, rows, columns);

        srand(time(NULL));
//        int array[5][5] = {{0}};
//
//        int r = 3, c = 4, i, j, count;

        int tempArr[rows*columns];

        for (int i = 0; i < rows*columns; i++)
                tempArr[i] = i;

        printf("FIRST\n");
        for (int i = 0; i < rows*columns; i++) {

            printf("%d ",tempArr[i]);
        }

        shuffle(tempArr,rows*columns);

        printf("SECOND\n");
        for (int i = 0; i < rows*columns; i++) {
            printf("%d ",tempArr[i]);
        }

        //TODO PUTING ANTS TO DISPLAY
        for (int i = 0; i < numberOfAnts; i++) {
            antsD[i].id = i+1;
            int position = tempArr[i];
            int x = position % columns;
            int y = position / columns;

            antsD[i].y = y;
            antsD[i].x = x;
            antsD[i].direction = NORTH;

            //chooseAntsPosition(rows,columns,&antsD[i]);
            antsD[i].display = &display;
            pthread_create(&ants[i],NULL,antF,&antsD[i]);
            printf("Created ant[%d] X:%d Y:%d\n",i+1,antsD[i].x,antsD[i].y);
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

        if(true) {
            printf("inside of file\n");
            //time and filename
            time_t t = time(NULL);
            struct tm tm = *localtime(&t);
            char fileNameString[50];
            //sprintf(fileNameString, "/home/gorny/writedFile_%02dhod_%02dmin.txt", tm.tm_mday, tm.tm_mon + 1, tm.tm_hour, tm.tm_min);
            sprintf(fileNameString, "/home/gorny/temp.txt");

            //Writing to file
            FILE *fptr;

            //fptr = fopen("../txtFiles/writedFile-.txt","w");
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
            printf("FILE WRITED ONTO SERVER\n");
            //sleep(2);
            //pthread_cond_broadcast(&pdata->updateClients);
        }

        printf("111\n");



        printf("PRED BROADCASTOM 1\n");
        pdata->step++;
        printData(pdata);

        if(pdata->stop != 1) {
            pthread_cond_broadcast(&pdata->updateClients);
            pthread_cond_wait(&pdata->continueAntSimulation,&pdata->mutex);
        }


        printf("33333\n");
        printf("step %d, down %d\n",pdata->step,pdata->download);
        //data_initServer(pdata, NULL);
        //TODO SPRAVIT po stahovani suboru a tiez broadcast

        printf("444\n");


        //printf("555\n");
        //sleep(1);
        //printf("PRED BROADCASTOM 2\n");
//        printData(pdata);
//        pthread_cond_broadcast(&pdata->updateClients);
        printf("66666\n");


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

        if(loadingType == TERMINAL_INPUT) {
            for (int i = 0; i <  display.dataSocket->rows ; i++) {
                free(display.dataSocket->colorOfDisplay[i]);
            }
            free(display.dataSocket->colorOfDisplay);
        }



        pthread_mutex_destroy(&mainMut);
        //Destroying barrier
        for (int i = 0; i < numberOfAnts; i++) {
            pthread_barrier_destroy(&barriers[i]);
        }
        if (*dataStop == 1) {
            repeat = false;
        }


        free(dataStop);
        printf("END OF REPEATING SIMULATION \n");
        printData(pdata);
        pthread_cond_broadcast(&pdata->updateClients);
        pthread_mutex_unlock(&pdata->mutex);
        printf("END OF REPEATING SIMULATION \n");
        if(pdata->stop == 1) {
            printf("break\n");
            break;
        }
        //pthread_cond_wait(&pdata->continueAntSimulation,&pdata->mutex);
    }
    printf("END OF SIMULATION \n");
    return EXIT_SUCCESS;
}

int main(int argc,char* argv[]) {

    ////SERVER
    printLogServer("int main(int argc,char* argv[])",1);
    if (argc < 4) {
        printError("Sever have to be launched with following arguments: port username typeOfCollision.");
    }
    int port = atoi(argv[1]);
    if (port <= 0) {
        printError("Port have to be integer greater than 0.");
    }
    char *userName = argv[2];
    COLLISION typeOfCollision = atoi(argv[3]);

    int serverSocket, clientSocket, addr_size;
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

    if(typeOfCollision <= 0 || typeOfCollision > 3) {
        typeOfCollision = 1;
    }

    switch (typeOfCollision) {
        case 1:
            printf("Collision of ants: Only one ant will survive.\n");
            break;
        case 2:
            printf("Collision of ants: All ants will die on collision.\n");
            break;
        case 3:
            printf("Collision of ants: After hard hit on collision, half of ants will continue only to south.\n");
            break;
        default:
            printf("No collision is set.\n");
    }

    //wait for, and eventually accept an incoming connection
    addr_size = sizeof(SA_IN);


    //inicializacia dat zdielanych medzi vlaknami
    DATA data;
    data.typeOfCollision = typeOfCollision;
    data_initServer(&data, userName);



    //vytvorenie condArray pre klientov
    //pthread_cond_t startListening[SERVER_BACKLOG];
    //data.condStartListeningArray = startListening;

    //vytvorenie arrayu pre sockety
    data.sockets = (int *) calloc(SERVER_BACKLOG, sizeof(int));
    data.sockets[0] = serverSocket;

    //inicializacia vlakna na ktorom bude prebiehat simulacia
    pthread_t gameThread;
    pthread_create(&gameThread, NULL, antSimulation, (void *) &data);

    //vytvorenie vlakna pre zapisovanie dat do socketu <pthread.h>
    pthread_t threadWrite[SERVER_BACKLOG];

    //vytvorenie vlakna  na citanie dat zo socketu
    pthread_t threadRead[SERVER_BACKLOG];

    //v hlavnom vlakne sa budu prijimat novi clienti
    while (true) {
        printLogServer("MAIN: while (true)",2);
        pthread_mutex_lock(&data.mutex);
        int numberOfConnections = data.numberOfConnections;
        pthread_mutex_unlock(&data.mutex);

        //printf("Pred acceptom\n");
        if ((clientSocket = accept(serverSocket, (SA *) &client_addr, (socklen_t *) &addr_size)) < 0) {
            printError("Error - accept");
            break;
        } else {
            printLogServer("Accepted socket",2);
            printf("Number of connections:%d\n",numberOfConnections);
            //akcepnuty socket
            pthread_mutex_lock(&data.mutex);
            data.sockets[numberOfConnections] = clientSocket;
            //poslane aktualne data
            writeToSocketActualData(&data, data.sockets[numberOfConnections]);
            //navysenie pre dalsieho klienta
            data.numberOfConnections++;
            pthread_mutex_unlock(&data.mutex);

            //vytvorenie vlakna pre posielanie dat klientovi
            pthread_create(&threadWrite[numberOfConnections-1], NULL, data_writeDataServer, (void *) &data);
            //vytvorenie vlakna pre citanie dat od klienta
            pthread_create(&threadRead[numberOfConnections-1], NULL, data_readDataServer, (void *) &data);
            //pthread_cond_init(&data.condStartListeningArray[numberOfConnections], NULL);

            printf("Client[%d] connected(socket%d)\n", numberOfConnections, clientSocket);

            if(data_isStopped(&data)) {
                break;
            }
        }
    }



    //pockame na skoncenie zapisovacieho vlakna <pthread.h> a vlakna na simulacia mravcov
    pthread_join(gameThread, NULL);
    printf("PTHREAD JOIN: gameThread\n");

    for (int i = 0; i < SERVER_BACKLOG; i++) {
        printf("PTHREAD JOIN: threadRead[%d]\n",i);
        pthread_join(threadRead[i], NULL);
    }

    for (int i = 0; i < SERVER_BACKLOG; i++) {
        printf("PTHREAD JOIN: threadWrite[%d]\n",i);
        pthread_join(threadWrite[i], NULL);
    }



    for (int i = 0; i < SERVER_BACKLOG; i++) {
        //uzatvorenie pasivneho socketu sockets[0]
        //uzavretie socketu klienta sockets[1+]
        printf("CLOSE: data.sockets[%d]\n",i);
        close(data.sockets[i]);
    }

    data_destroyServer(&data);

    return (EXIT_SUCCESS);

    ////END-SERVER
}