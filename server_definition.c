//
// Created by Maros Gorny on 28. 12. 2022.
//

#include "server_definition.h"
#include "client_server_definitions.h"
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <malloc.h>
#include <stdlib.h>


char* endMsg = ":end";
char* pauseMsg = ":pause";
char* continueMsg = ":continue";
char* quitMsg = ":quit";


/**
 * Posle vsetky dolezite udaje z DATA* do socketu.
*
* @param  data DATA* - zdielane data
 * @param  socket int - socket kam budu poslane data
* @return void void pointer
*/
void writeToSocketActualData(DATA* pdata, int socket){
    printLogServer(" writeToSocketActualData(DATA* pdata, int socket)",1);
    char buffer[BUFFER_LENGTH + 1];
    buffer[BUFFER_LENGTH] = '\0';
    char* action = "[Shared date state]";
    sprintf(buffer, "%s%s: %d %d %d %d %d %d %d %d %d %d %s\n"
                                                        ,pdata->userName\
                                                        ,action\
                                                        ,pdata->numberOfAnts\
                                                        ,pdata->loadingType\
                                                        ,pdata->logicType\
                                                        ,pdata->rows\
                                                        ,pdata->columns\
                                                        ,pdata->stop\
                                                        ,pdata->continueSimulation\
                                                        ,pdata->step\
                                                        ,pdata->ready\
                                                        ,pdata->download\
                                                        ,pdata->txtFileName);
    char *pos = strchr(buffer, '\n');
    if (pos != NULL) {
        *pos = '\0';
    }
    write(socket, buffer, strlen(buffer) + 1);
    printf("Data sent: %s\n",buffer);
};


void *data_writeDataServer(void *data) {
    printLogServer("void *data_writeData(void *data)",1);
    DATA *pdata = (DATA *)data;

    pthread_mutex_lock(&pdata->mutex);
    int idClient = pdata->numberOfConnections - 1;
    pthread_mutex_unlock(&pdata->mutex);

    //pre pripad, ze chceme poslat viac dat, ako je kapacita buffra
    fcntl(STDIN_FILENO, F_SETFL, fcntl(STDIN_FILENO, F_GETFL, 0) | O_NONBLOCK);
    //printData(pdata);


    while(!data_isStopped(pdata)) {
        pthread_mutex_lock(&pdata->mutex);
        printf("WRITE CAKAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA na WAIT\n");
        pthread_cond_wait(&pdata->updateClients,&pdata->mutex);
        printf("WRITE ZA WAAAAAAAAAAAAAAAAITOM\n");
        if(pdata->stop ==1) {
            pthread_mutex_unlock(&pdata->mutex);
            break;
        }


        printf("STEP IN WRITEDATA:%d %d\n",pdata->step,pdata->download);
        if(pdata->step == 9 && pdata->download != 0) {
            //writeToSocketActualData(pdata, pdata->sockets[idClient]);
            if(pdata->download == 2) {
                writeToSocketActualData(pdata, pdata->sockets[idClient]);
                usleep(500);
                send_fileServer(pdata->sockets[idClient],pdata);
            }

            if(pdata->download == 1) {
                //time and filename
                time_t t = time(NULL);
                struct tm tm = *localtime(&t);
                char fileNameString[50];
                sprintf(fileNameString, "%s/savedFileServer_%04d%02d%02d_%02d%02d.txt",getPWD(),tm.tm_year+1900, tm.tm_mday, tm.tm_mon + 1, tm.tm_hour, tm.tm_min);

                char oldName[50];
                sprintf(oldName,"%s/temp.txt",getPWD());
                printf("oldName: %s\n",oldName);
                rename(oldName,fileNameString);
                printf("File is saved on server with name:%s\n",fileNameString);
                printf("RENAMED\n");
                pdata->step++;
                writeToSocketActualData(pdata, pdata->sockets[idClient]);
            } else if (pdata->download == 2) {
                char oldName[50];
                sprintf(oldName,"%s/temp.txt",getPWD());
                remove(oldName);
            }





        } else {
            writeToSocketActualData(pdata, pdata->sockets[idClient]);
        }



        pthread_mutex_unlock(&pdata->mutex);
    }

    //startSendingDataToServer(pdata);
    //writeMsgToAllParticipants(pdata);
    fcntl(STDIN_FILENO, F_SETFL, fcntl(STDIN_FILENO, F_GETFL, 0) & ~O_NONBLOCK);

    printf("RETURNNNNN data_writeDataServer\n");
    return NULL;
}

void *data_readDataServer(void *data) {
    printLogServer("void *data_readData(void *data)",1);
    DATA *pdata = (DATA *)data;

    pthread_mutex_lock(&pdata->mutex);
    int idClient = pdata->numberOfConnections - 1;
    int socket = pdata->sockets[idClient];
    pthread_mutex_unlock(&pdata->mutex);

    printf("ID client %d\n",idClient);

    char buffer[BUFFER_LENGTH + 1];
    buffer[BUFFER_LENGTH] = '\0';
    int n = 0;

    //printf("BEFORE WHILE\n");
    while(!data_isStopped(pdata)) {
        printf("data_read stop: %d\n",pdata->stop);

        //printf("Client:%d INSIDE WHILE\n",idClient);
        bzero(buffer, BUFFER_LENGTH);
        if (read(socket, buffer, BUFFER_LENGTH) > 0) {
            //printf("Client:%d INSIDE READ\n",idClient);

            makeActionNew(buffer,pdata);

            //processReadData(buffer,pdata);

            printf("NNNNNNNNNNNNNNNNNNNNNNNNNNNNN:%d\n",n);
            printf("STEEEEEEEEEEEEEEEEEEEEEEEEEEP:%d\n",pdata->step);


            printf("pthread_cond_broadcast(&pdata->updateClients);\n");
            pthread_mutex_lock(&pdata->mutex);
            pthread_cond_broadcast(&pdata->updateClients);
            pthread_mutex_unlock(&pdata->mutex);
//            if(pdata->step == 5)
//                n++;
//            if(pdata->step == 5 && n > 1) {
//
//            } else {
//
//            }




        }
        else {
            printf("Client[%d] Cann\'t read a socket\n",idClient);
            data_stop(pdata);
        }
    }
    pthread_cond_signal(&pdata->startAntSimulation);
    printf("RETURNNNNN data_readDataServer\n");
    return NULL;
}


void makeActionNew(char* buffer, DATA *pdata) {
    printLogServer("void makeActionNew(char* buffer, DATA *pdata)",1);
    printf("PRISLA SPRAVA:%s\n",buffer);

    if(pdata->stop == 1) {
        printf("READ THREAD SHUTED DOWN\n");
        return;
    }
//    char* posSemi = strchr(buffer, ':');
//    char* posSemiSecond = strchr(posSemi+1, ':');
    if(!semicolonAction(buffer,pdata)) {
        //IF NOT SEMICOLON ACTION
        printf("STEP:%d\n",pdata->step);

        char *target = NULL;
        char *posActionBracketsStart = strchr(buffer, '[');
        char *posActionBracketsEnd = strchr(buffer, ']');

//        if(pdata->step == 5) {
//
//            if(pdata->loadingType == FILE_INPUT_LOCAL) {
//                printf("INSIDE FILE INPUT_LOCAL\n");
//                write_file(pdata->sockets[1]);
//                printf("OUTSIDE FILE INPUT_LOCAL\n");
//            }
//        } else
        if (posActionBracketsStart != NULL) {

            target = (char *) malloc(posActionBracketsEnd - posActionBracketsStart + 2);
            memcpy(target, posActionBracketsStart, posActionBracketsEnd - posActionBracketsStart + 1);
            target[posActionBracketsEnd - posActionBracketsStart + 1] = '\0';

            printf("Bracket Action: %s\n", target);

            //printData(pdata);

            bool addStep = true;
            pthread_mutex_lock(&pdata->mutex);
            if (strcmp(target, "[Number of ants]") == 0) {
                pdata->numberOfAnts = atoi(posActionBracketsEnd + 2);
                printf("Changed number of ants: %d\n", pdata->numberOfAnts);
            } else if (strcmp(target, "[Loading type]") == 0) {
                LOADING_TYPE loadingType = (LOADING_TYPE) atoi(posActionBracketsEnd + 2) - 1;
                pdata->loadingType = loadingType;
                printf("Changed loading type: %d\n", pdata->loadingType);
            } else if (strcmp(target, "[Logic type]") == 0) {
                LOGIC_TYPE logicType = (LOGIC_TYPE) atoi(posActionBracketsEnd + 2) - 1;
                pdata->logicType = logicType;
                printf("Changed logic type: %d\n", pdata->logicType);

                if(pdata->loadingType == FILE_INPUT_LOCAL) {
                    pdata->step++;
                }

            } else if (strcmp(target, "[Dimensions]") == 0) {


                char *columnsCharPointer = strchr(posActionBracketsEnd + 3, ' ');
                if (columnsCharPointer) printf("%s\n", columnsCharPointer);
                if (columnsCharPointer == NULL) {
                    pdata->columns = atoi(posActionBracketsEnd + 2);
                } else {
                    pdata->columns = atoi(columnsCharPointer);
                }
                pdata->rows = atoi(posActionBracketsEnd + 2);
                printf("Changed rows: %d columns: %d\n", pdata->rows, pdata->columns);

                if(pdata->loadingType == TERMINAL_INPUT) {
                    // init colorOfDisplay
                    printf("ALLOCATION 1\n");

                    printf("ALLOCATION 2\n");
                    pdata->colorOfDisplay = (int**) calloc(pdata->rows, sizeof(int*));
                    for (int i = 0; i <  pdata->rows ; i++) {
                        printf("ALLOCATION 3\n");
                        pdata->colorOfDisplay[i] = (int*) calloc(pdata->columns,sizeof(int));
                    }

                    addStep = false;
                }
                pdata->step++;



            } else if (strcmp(target, "[SELECTING BLACK BOXES]") == 0) {
                printf("Dosal sa dnu\n");
                printf("buf: %s\n",buffer);
                char *yPointer = strchr(posActionBracketsEnd + 3, ' ');
                if(strstr(posActionBracketsEnd+2, "OK") == NULL) {
                    printf("Dosal sa dnu1\n");
                    int y = atoi(yPointer);
                    int x = atoi(posActionBracketsEnd + 2);
                    printf("Dosal sa dnu3\n");
                    pdata->colorOfDisplay[x][y] = 1;
                    printf("added black box to position X: %d Y: %d\n", y, x);
                    addStep = false;
                }
                pdata->step++;


            } else if (strcmp(target,"[FILENAME]") == 0)  {
                addStep = false;
                printf("else if (strcmp(target,\"[FILENAME]\") == 0)\n");
                printf("TXT FILE PDATA %s\n",pdata->txtFileName);
                strcpy(pdata->txtFileName,posActionBracketsEnd+3);
                printf("TXT FILE PDATA %s\n",pdata->txtFileName);



            } else if (strcmp(target,"[FileL]") == 0) {
//                if(pdata->step == 5) {
//                    pdata->step++;
//                }

               // write_file(pdata->sockets[1]);

                addStep = false;
                printf("else if (strcmp(target,\"[FileL]\") == 0)\n");
                FILE *fptr;
                //fptr = fopen("../txtFiles/writedFile-.txt","w");

                if ((fptr = fopen("/home/gorny/temp.txt","a")) == NULL){
                    printf("Error! opening file");
                }
                if(fptr == NULL) {
                    printf("Error writing\n");
                } else {
                    printf("writed:%s\n",posActionBracketsEnd+2);
                    printf("%s\n",posActionBracketsEnd+3);
                    if(strcmp(posActionBracketsEnd+3,"END") == 0) {
                        addStep = true;
                        //fprintf(fptr,"%s", posActionBracketsEnd + 2);
                    } else {
                        fprintf(fptr,"%s", posActionBracketsEnd + 2);
                    }

                }
                printf("CLOSING FILE\n");
                fclose(fptr);
                printf("CLOSED FILE\n");


//                if(addStep) {
//                    int rows;
//                    int columns;
//                    fptr = fopen("/home/gorny/temp.txt","r");
//                    fscanf(fptr,"%d", &rows);
//                    fscanf(fptr,"%d", &columns);
//
//                }



            } else if (strcmp(target, "[READY]") == 0) {
                char *columnsCharPointer = strchr(posActionBracketsEnd, ' ');
                int temp = atoi(columnsCharPointer);
                if (temp == 1) {
                    printf("ANTS ARE READY\n");
                    pthread_cond_signal(&pdata->startAntSimulation);
                } else {
                    addStep = false;
                    printf("ANTS ARE NOT READY\n");
                }
            } else if (strcmp(target,"[DOWNLOAD]") == 0) {
                //TODO SPRAVIT STAHOVANIE SUBORU
                int temp = atoi(posActionBracketsEnd + 2);
                pdata->download = temp;
                printf("Download changed to: %d\n", pdata->download);

                if(temp == 1) {   // server
                    pthread_cond_signal(&pdata->continueAntSimulation);
                } else if (temp == 2) { //local
                    pthread_cond_signal(&pdata->continueAntSimulation);
                } else if (temp == 3) { // nowhere
                    //TODO NOTHING
                }

            } else if (strcmp(target,"[END]") == 0) {
                printf("DOSTAL SA DO ENDU\n");
                int temp = atoi(posActionBracketsEnd + 2);

                if(temp == 1) {   // server
                    printf("DOSTAL SA DO ENDU1\n");
                    data_initServer(pdata, NULL);
                    addStep = false;
                } else if (temp == 2) { //local
                    printf("DOSTAL SA DO ENDU2\n");
                    pthread_mutex_unlock(&pdata->mutex);
                    data_stop(pdata);
                    printf("DOSTAL SA PRED SIGNAL\n");
                    pthread_cond_broadcast(&pdata->startAntSimulation);
                    pthread_cond_broadcast(&pdata->updateClients);
                    printf("DOSTAL SA ZA SIGNAL\n");

                    pthread_mutex_lock(&pdata->mutex);
                    shutdown(pdata->sockets[0],2);
                    printf("shutdown sockets[0]\n");
                    shutdown(pdata->sockets[1],2);
                    printf("shutdown sockets[1]\n");
                }

            }
            //Add step
            if (addStep) pdata->step++;
            if(addStep) printf("ADDED STEP ++\n");
            pthread_mutex_unlock(&pdata->mutex);
            free(target);
            printf("Dosal sa von\n");
        }
    }
}

bool semicolonAction(char* buffer, DATA *pdata) {

    char* posSemi = strchr(buffer, ':');
    char* posSemiSecond = strchr(posSemi+1, ':');

    //IF SEMICOLON ACTION
    if(posSemiSecond != NULL) {
        if(posSemiSecond - posSemi == 2) {
            char* userSemiAction;
            bool match = true;
            if(strcmp(posSemiSecond,endMsg) == 0) {
                userSemiAction = "ukoncil komunikaciu";
                data_stop(pdata);
                shutdown(pdata->sockets[0],2);
            } else if (strcmp(posSemiSecond,pauseMsg) == 0) {
                userSemiAction = "pauzol simulaciu";
                pthread_mutex_lock(&pdata->mutex);
                pdata->continueSimulation = 0;
//                printf("Contunie simulation = 0\n");
                pthread_mutex_unlock(&pdata->mutex);
            } else if (strcmp(posSemiSecond,continueMsg) == 0) {
                userSemiAction = "odpauzoval simulaciu";
                pthread_mutex_lock(&pdata->mutex);
                pdata->continueSimulation = 1;
                pthread_cond_broadcast(&pdata->continueAntSimulation);
                pthread_mutex_unlock(&pdata->mutex);
            } else if (strcmp(posSemiSecond,quitMsg) == 0) {
                userSemiAction = "opustil komunikaciu";
                //TODO DOROBIT LEN QUIT
            } else {
                match = false;
            }

            if(match) {
                *(posSemiSecond - 2) = '\0';
                printf("Pouzivatel %s %s.\n", buffer, userSemiAction);
            } else {
                return false;
            }
        } else {
            return false;
        }
    } else {
        return false;
    }
    return true;
}


void data_destroyServer(DATA *data) {
    pthread_mutex_destroy(&data->mutex);
    pthread_mutex_destroy(&data->writtenMutex);

    pthread_cond_destroy(&data->startAntSimulation);
    pthread_cond_destroy(&data->continueAntSimulation);
    pthread_cond_destroy(&data->updateClients);




//    for (int i = 0; i < SERVER_BACKLOG; i++) {
//        pthread_cond_destroy(&data->condStartListeningArray[i]);
//    }
    free(data->sockets);
}

/**
 * Inicializacia zdielanych dat na serveri.
 * Pokial je username NULL, znamena to, ze data sa len resetuju,
 * takze niektore kroky sa mozu vynechat.
*
* @param  data DATA* - zdielane data
 * @param  userName char* - meno servera
* @return void
*/
void data_initServer(DATA *data, const char* userName) {
    printLogServer("void data_initServer(DATA *data, const char* userName)",1);

    if(userName != NULL) {
        data->userName[USER_LENGTH] = '\0';
        strncpy(data->userName, userName, USER_LENGTH);
        data->continueSimulation = 1;
        data->numberOfConnections = 1;

        //mutexes init
        pthread_mutex_init(&data->mutex, NULL);
        pthread_mutex_init(&data->writtenMutex, NULL);

        //conds init
        pthread_cond_init(&data->startAntSimulation, NULL);
        pthread_cond_init(&data->continueAntSimulation, NULL);
        pthread_cond_init(&data->updateClients, NULL);
    }


    //Simulation data
    data->numberOfAnts = 0;
    data->loadingType = NOT_SELECTED_LOADING_TYPE;
    data->logicType = NOT_SELECTED_ANTS_LOGIC;
    data->rows = 0;
    data->columns = 0;

    //Communication data
    data->stop = 0;
    data->step = 1;
    data->ready = 0;
    data->txtFileName[99] = '\0';
    strncpy(data->txtFileName, "NULL", USER_LENGTH);
}

void write_file(int socket) {
    //printLogServer("void write_file(int socket)");
    int n;
    FILE *fptrRead;
    char *filename = "prijate.txt";
    char buffer[BUFFER_LENGTH];
    fptrRead = fopen(filename, "w");
    while (1) {
        printLogServer("REAAD WHILE 1",2);
        n = read(socket, buffer, BUFFER_LENGTH);
        if (n <= 0) {
            break;
            return;
        }
        fprintf(fptrRead, "%s", buffer);
        printf("%s\n",buffer);
        bzero(buffer, BUFFER_LENGTH);
    }

}

void send_fileServer(int socket,DATA* pdata) {
    printLogServer("void send_fileServer(int socket,DATA* pdata)",1);

    FILE *fptrRead;
    if ((fptrRead = fopen("/home/gorny/temp.txt","r")) == NULL){
        printf("Error! opening file");
    } else {
        char buffer[BUFFER_LENGTH + 1];
        buffer[BUFFER_LENGTH] = '\0';
        char *pos = strchr(buffer, '\n');
        if (pos != NULL) {
            *pos = '\0';
        }


        char* action = "[FileS]";

        int userNameLength = strlen(pdata->userName);
        int countCharAfterName = 2 + strlen(action);
        sprintf(buffer, "%s%s: ", pdata->userName,action);
        char *textStart = buffer + (userNameLength + countCharAfterName);



        int n = 0;
        while (fgets(textStart, BUFFER_LENGTH - (userNameLength + countCharAfterName), fptrRead) != NULL)  {
            printf("FILE TO WRITE: %s\n",buffer);
            usleep(500);
            if(write(socket, buffer, strlen(buffer) + 1) < 0) {
                printf("ERRRRRRRRRRR\n");
            } else {
                printf("Successfull\n");
            }
        }
        bzero(buffer, BUFFER_LENGTH);
        buffer[BUFFER_LENGTH] = '\0';
        pos = strchr(buffer, '\n');
        if (pos != NULL) {
            *pos = '\0';
        }

        action = "[FileS]";

        userNameLength = strlen(pdata->userName);
        countCharAfterName = 2 + strlen(action);
        sprintf(buffer, "%s%s: END", pdata->userName,action);
        textStart = buffer + (userNameLength + countCharAfterName);

        printf("poslanu buffffiiiiiik:%s\n",buffer);
        usleep(500);
        if(write(socket, buffer, strlen(buffer) + 1) < 0) {
            printf("ERROR POSLAT END\n");
        } else {
            printf("POSLANYYYYYY END\n");
        }

    }
    fclose(fptrRead);


    pdata->step++;
    printLogServer("OUT OF: void send_fileServer(FILE *pFile, int socket)",0);
}








