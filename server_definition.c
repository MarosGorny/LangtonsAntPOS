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
};


void *data_writeDataServer(void *data) {
    printLogServer("void *data_writeData(void *data)",1);
    DATA *pdata = (DATA *)data;

    pthread_mutex_lock(&pdata->mutex);
    int idClient = pdata->numberOfConnections - 1;
    pthread_mutex_unlock(&pdata->mutex);

    //pre pripad, ze chceme poslat viac dat, ako je kapacita buffra
    fcntl(STDIN_FILENO, F_SETFL, fcntl(STDIN_FILENO, F_GETFL, 0) | O_NONBLOCK);


    while(!data_isStopped(pdata)) {
        pthread_mutex_lock(&pdata->mutex);
        pthread_cond_wait(&pdata->updateClients,&pdata->mutex);
        if(pdata->stop ==1) {
            pthread_mutex_unlock(&pdata->mutex);
            break;
        }


        if(pdata->step == 9 && pdata->download != 0) {
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
                rename(oldName,fileNameString);
                printf("File is saved on server with name:%s\n",fileNameString);
                pdata->step++;
                writeToSocketActualData(pdata, pdata->sockets[idClient]);
            } else if (pdata->download == 2) {
                char oldName[50];
                sprintf(oldName,"%s/temp.txt",getPWD());
                remove(oldName);
            } else {
                pdata->step++;
                writeToSocketActualData(pdata, pdata->sockets[idClient]);
            }

        } else {
            writeToSocketActualData(pdata, pdata->sockets[idClient]);
        }

        pthread_mutex_unlock(&pdata->mutex);
    }
    fcntl(STDIN_FILENO, F_SETFL, fcntl(STDIN_FILENO, F_GETFL, 0) & ~O_NONBLOCK);

    return NULL;
}

void *data_readDataServer(void *data) {
    printLogServer("void *data_readData(void *data)",1);
    DATA *pdata = (DATA *)data;

    pthread_mutex_lock(&pdata->mutex);
    int idClient = pdata->numberOfConnections - 1;
    int socket = pdata->sockets[idClient];
    pthread_mutex_unlock(&pdata->mutex);


    char buffer[BUFFER_LENGTH + 1];
    buffer[BUFFER_LENGTH] = '\0';

    while(!data_isStopped(pdata)) {

        bzero(buffer, BUFFER_LENGTH);
        if (read(socket, buffer, BUFFER_LENGTH) > 0) {

            makeActionNew(buffer,pdata);

            pthread_mutex_lock(&pdata->mutex);
            pthread_cond_broadcast(&pdata->updateClients);
            pthread_mutex_unlock(&pdata->mutex);

        }
        else {
            printf("Client[%d] Cann\'t read a socket\n",idClient);
            data_stop(pdata);
        }
    }
    pthread_mutex_lock(&pdata->mutex);
    pthread_cond_signal(&pdata->startAntSimulation);
    pthread_cond_broadcast(&pdata->updateClients);
    pthread_mutex_unlock(&pdata->mutex);
    return NULL;
}


void makeActionNew(char* buffer, DATA *pdata) {
    printLogServer("void makeActionNew(char* buffer, DATA *pdata)",1);

    if(pdata->stop == 1) {
        return;
    }

    if(!semicolonAction(buffer,pdata)) {
        //IF NOT SEMICOLON ACTION

        char *target = NULL;
        char *posActionBracketsStart = strchr(buffer, '[');
        char *posActionBracketsEnd = strchr(buffer, ']');

        if (posActionBracketsStart != NULL) {

            target = (char *) malloc(posActionBracketsEnd - posActionBracketsStart + 2);
            memcpy(target, posActionBracketsStart, posActionBracketsEnd - posActionBracketsStart + 1);
            target[posActionBracketsEnd - posActionBracketsStart + 1] = '\0';

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

                if(pdata->loadingType == FILE_INPUT_LOCAL || pdata->loadingType == FILE_INPUT_SERVER) {
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
                    pdata->colorOfDisplay = (int**) calloc(pdata->rows, sizeof(int*));
                    for (int i = 0; i <  pdata->rows ; i++) {
                        pdata->colorOfDisplay[i] = (int*) calloc(pdata->columns,sizeof(int));
                    }
                    addStep = false;
                } else {
                    pdata->step++;
                }
            } else if (strcmp(target, "[SELECTING BLACK BOXES]") == 0) {

                char *yPointer = strchr(posActionBracketsEnd + 3, ' ');
                if(strstr(posActionBracketsEnd+2, "OK") == NULL) {
                    int y = atoi(yPointer);
                    int x = atoi(posActionBracketsEnd + 2);
                    pdata->colorOfDisplay[x][y] = 1;
                    printf("Added black box to position X: %d Y: %d\n", y, x);
                    addStep = false;
                } else {
                    pdata->step++;
                }



            } else if (strcmp(target,"[FILENAME]") == 0)  {
                addStep = false;
                strcpy(pdata->txtFileName,posActionBracketsEnd+3);

                if(pdata->loadingType == FILE_INPUT_SERVER) {
                    pdata->step++;
                } else if(pdata->loadingType == FILE_INPUT_LOCAL) {
                    char tempFileNameString[50];
                    sprintf(tempFileNameString, "%s/temp.txt", getPWD());
                    remove(tempFileNameString);
                }



            } else if (strcmp(target,"[FileL]") == 0) {

                addStep = false;
                printf("else if (strcmp(target,\"[FileL]\") == 0)\n");
                FILE *fptr;

                char tempFileNameString[50];
                sprintf(tempFileNameString, "%s/temp.txt", getPWD());


                if ((fptr = fopen(tempFileNameString,"a")) == NULL){
                    printf("Error! opening file");
                }
                if(fptr == NULL) {
                    printf("Error writing\n");
                } else {
                    if(strcmp(posActionBracketsEnd+3,"END") == 0) {
                        addStep = true;
                    } else {
                        fprintf(fptr,"%s", posActionBracketsEnd + 2);
                    }
                }
                fclose(fptr);



            } else if (strcmp(target, "[READY]") == 0) {
                char *columnsCharPointer = strchr(posActionBracketsEnd, ' ');
                int temp = atoi(columnsCharPointer);
                if (temp == 1) {
                    //printf("ANTS ARE READY\n");
                    pthread_cond_signal(&pdata->startAntSimulation);
                } else {
                    addStep = false;
                    //printf("ANTS ARE NOT READY\n");
                }
            } else if (strcmp(target,"[DOWNLOAD]") == 0) {
                int temp = atoi(posActionBracketsEnd + 2);
                pdata->download = temp;
                printf("Download changed to: %d\n", pdata->download);

                if(temp == 1) {   // server
                    pthread_cond_signal(&pdata->continueAntSimulation);
                } else if (temp == 2) { //local
                    pthread_cond_signal(&pdata->continueAntSimulation);
                } else if (temp == 3) { // nowhere
                    pthread_cond_signal(&pdata->continueAntSimulation);
                }

            } else if (strcmp(target,"[END]") == 0) {
                int temp = atoi(posActionBracketsEnd + 2);

                if(temp == 1) {   // server
                    data_initServer(pdata, NULL);
                    addStep = false;
                } else if (temp == 2) { //local
                    pthread_mutex_unlock(&pdata->mutex);

                    data_stop(pdata);
                    shutdown(pdata->sockets[0],2);
                }
            }
            //Add step
            if (addStep) pdata->step++;

            pthread_mutex_unlock(&pdata->mutex);
            free(target);
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

    free(data->sockets);
}


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

    memset(data->txtFileName, 0, sizeof data->txtFileName);
    data->txtFileName[99] = '\0';
    strncpy(data->txtFileName, "NULL", USER_LENGTH);
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
            usleep(300);
            if(write(socket, buffer, strlen(buffer) + 1) < 0) {
                printf("Can't send information\n");
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


        usleep(1000);
        if(write(socket, buffer, strlen(buffer) + 1) < 0) {
            printf("Can't send information\n");
        }
    }
    fclose(fptrRead);

    pdata->step++;
}









