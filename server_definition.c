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
#include <errno.h>

char* endMsg = ":end";
char* pauseMsg = ":pause";
char* continueMsg = ":continue";
char* quitMsg = ":quit";

void writeStateOfSharedData(DATA* pdata, int socket){
    printLogServer("void writeStateOfSharedData(DATA* pdata, int socket)");
    char buffer[BUFFER_LENGTH + 1];
    buffer[BUFFER_LENGTH] = '\0';
    char* action = "[Shared date state]";
    sprintf(buffer, "%s%s: %d %d %d %d %d %d %d %d %d\n"
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
                                                        ,pdata->ready);
                                                        //TODO Ak by som chcel na poziciu blacBox pridat x a y akontrolovat zmenu
    char *pos = strchr(buffer, '\n');
    if (pos != NULL) {
        *pos = '\0';
    }
    printf("Pred writom\n");
    write(socket, buffer, strlen(buffer) + 1);
    printf("State data sended: %s\n",buffer);
};


void *data_writeDataServer(void *data) {
    printLogServer("void *data_writeData(void *data)");
    DATA *pdata = (DATA *)data;

    pthread_mutex_lock(&pdata->mutex);
    int idClient = pdata->numberOfClients - 1;
    pthread_mutex_unlock(&pdata->mutex);

    //pre pripad, ze chceme poslat viac dat, ako je kapacita buffra
    fcntl(STDIN_FILENO, F_SETFL, fcntl(STDIN_FILENO, F_GETFL, 0) | O_NONBLOCK);
    //printData(pdata);


    while(!data_isStopped(pdata)) {
        pthread_mutex_lock(&pdata->mutex);
        pthread_cond_wait(&pdata->updateClients,&pdata->mutex);
        writeStateOfSharedData(pdata,pdata->sockets[idClient]);
        pthread_mutex_unlock(&pdata->mutex);
    }

    //initSimulationSetting(pdata);
    //writeMsgToAllParticipants(pdata);
    fcntl(STDIN_FILENO, F_SETFL, fcntl(STDIN_FILENO, F_GETFL, 0) & ~O_NONBLOCK);

    return NULL;
}

void *data_readDataServer(void *data) {
    printLogServer("void *data_readData(void *data)");
    DATA *pdata = (DATA *)data;

    pthread_mutex_lock(&pdata->mutex);
    int idClient = pdata->numberOfClients - 1;
    int socket = pdata->sockets[idClient];
    pthread_mutex_unlock(&pdata->mutex);

    printf("ID client %d\n",idClient);

    char buffer[BUFFER_LENGTH + 1];
    buffer[BUFFER_LENGTH] = '\0';

    //printf("BEFORE WHILE\n");
    while(!data_isStopped(pdata)) {
        printf("data_read stop: %d\n",pdata->stop);
        int n = 0;
        //printf("Client:%d INSIDE WHILE\n",idClient);
        bzero(buffer, BUFFER_LENGTH);
        if (read(socket, buffer, BUFFER_LENGTH) > 0) {
            //printf("Client:%d INSIDE READ\n",idClient);

//            FILE *fptr;
//            //fptr = fopen("../txtFiles/writedFile-.txt","w");
//            time_t t = time(NULL);
//            struct tm tm = *localtime(&t);
//            char fileNameString[50];
//            sprintf(fileNameString, "/home/gorny/txt.txt");
//            if ((fptr = fopen(fileNameString,"w")) == NULL){
//                printf("Error! opening file");
//            } else {
//                fprintf(fptr,"%s", "ahoj\n");
//            }
//
//            printf("CLOSING FILE\n");
//            fclose(fptr);
//            printf("CLOSED FILE\n");

//#include <unistd.h>
//#include <sys/types.h>
//#include <pwd.h>
//
//            struct passwd pwd;
//            struct passwd *result;
//            char *buf;
//            size_t bufsize;
//            int s;
//            bufsize = sysconf(_SC_GETPW_R_SIZE_MAX);
//            if (bufsize == -1)
//                bufsize = 0x4000; // = all zeroes with the 14th bit set (1 << 14)
//            buf = malloc(bufsize);
//            if (buf == NULL) {
//                perror("malloc");
//                exit(EXIT_FAILURE);
//            }
//            s = getpwuid_r(getuid(), &pwd, buf, bufsize, &result);
//            if (result == NULL) {
//                if (s == 0)
//                    printf("Not found\n");
//                else {
//                    errno = s;
//                    perror("getpwnam_r");
//                }
//                exit(EXIT_FAILURE);
//            }
//            char *homedir = result->pw_dir;
//            printf("HOMEDIR %s\n",homedir);


            makeActionNew(buffer,pdata);

            //readMakeAction(buffer,pdata);

            if(pdata->step == 5)
                n++;
            if(pdata->step == 5 && n > 1) {

            } else {
                printf("pthread_cond_broadcast(&pdata->updateClients);\n");
                pthread_cond_broadcast(&pdata->updateClients);
            }




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
    printLogServer("void makeActionNew(char* buffer, DATA *pdata)");

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
                //TODO SPRAVIT ABY vyplo celu simulaciu
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




    for (int i = 0; i < SERVER_BACKLOG; i++) {
        pthread_cond_destroy(&data->condStartListeningArray[i]);
    }
    free(data->sockets);
}

void data_init(DATA *data, const char* userName) {
    printLogServer("void data_init(DATA *data, const char* userName, const int socket)");
    data->userName[USER_LENGTH] = '\0';
    if(userName != NULL) {
        strncpy(data->userName, userName, USER_LENGTH);
        data->continueSimulation = 1;
        data->numberOfClients = 1;

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


}

void write_file(int socket) {
    printLogServer("void write_file(int socket)");
    int n;
    FILE *fptrRead;
    char *filename = "prijate.txt";
    char buffer[BUFFER_LENGTH];
    fptrRead = fopen(filename, "w");
    while (1) {
        printLogServer("REAAD WHILE 1");
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








