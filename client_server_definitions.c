#include "client_server_definitions.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#include <unistd.h>
#include <fcntl.h>
#include <stdbool.h>
#include <netinet/in.h>
#include "settings.h"

char *endMsg = ":end";

void* writeMsgToAllParticipants(DATA* pdata) {
    char buffer[BUFFER_LENGTH + 1];
    buffer[BUFFER_LENGTH] = '\0';
    int userNameLength = strlen(pdata->userName);
    fd_set inputs;
    FD_ZERO(&inputs);
    struct timeval tv;
    tv.tv_usec = 0;
    while(!data_isStopped(pdata)) {
        tv.tv_sec = 1;
        FD_SET(STDIN_FILENO, &inputs);
        select(STDIN_FILENO + 1, &inputs, NULL, NULL, &tv);
        if (FD_ISSET(STDIN_FILENO, &inputs)) {

            sprintf(buffer, "%s: ", pdata->userName);
            char *textStart = buffer + (userNameLength + 2);
            while (fgets(textStart, BUFFER_LENGTH - (userNameLength + 2), stdin) > 0) {
                char *pos = strchr(textStart, '\n');
                if (pos != NULL) {
                    *pos = '\0';
                }
                write(pdata->socket, buffer, strlen(buffer) + 1);

                if (strstr(textStart, endMsg) == textStart && strlen(textStart) == strlen(endMsg)) {
                    printf("Koniec komunikacie.\n");
                    data_stop(pdata);
                }
            }
        }
    }
}

bool writeToSocketAndSetSharedAntsData(DATA* pdata,ACTION_CODE actionCode, char* buffer, char* textStart) {
    if(actionCode == NUMBER_OF_ANTS_ACTION) {
        int temp = atoi(textStart);
        if(temp > 0) {

            write(pdata->socket, buffer, strlen(buffer) + 1);
            //data_written(pdata);
            pdata->numberOfAnts = temp;
            return true;
        }
    } else if (actionCode == LOADING_TYPE_ACTION) {
        LOADING_TYPE tempLoadType = 100;
        switch (textStart[0]) {
            case '1':
                printf("All squares are white.\n");
                tempLoadType = ALL_WHITE;
                break;
            case'2':
                printf("Square color is random.\n");
                tempLoadType = RANDOM_COLOR;
                break;
            case'3':
                printf("Select black square through the terminal input.\n");
                tempLoadType = TERMINAL_INPUT;
                break;
            case'4':
                printf("Load dimension and squares color from file.\n");
                tempLoadType = FILE_INPUT;
                break;
            case'Q':
            case'q':
                printf("Closing simulation...\n");
                tempLoadType = NOT_SELECTED_LOADING_TYPE;
                break;
            default:
                printf("Selection is unknown. Try again please.\n");
                break;
        }
        if (tempLoadType != 100) {
            write(pdata->socket, buffer, strlen(buffer) + 1);
            //data_written(pdata);
            pdata->loadingType = tempLoadType;
            //reset_written(pdata);
            return true;
        }
    } else if (actionCode == LOGIC_TYPE_ACTION) {
        LOGIC_TYPE tempLogicType = 100;
        switch (textStart[0]) {
            case '1':
                printf("Logic of ants is direct.\n");
                tempLogicType = DIRECT;
                break;
            case '2':
                printf("Logic of ants is inverted.\n");
                tempLogicType =  INVERT;
                break;
            case 'q':
            case 'Q':
                printf("Closing simulation...\n");
                tempLogicType =  NOT_SELECTED_ANTS_LOGIC;
                break;
            default:
                printf("Selection is unknown. Try again please.\n");
                break;
        }
        if (tempLogicType != 100) {
            write(pdata->socket, buffer, strlen(buffer) + 1);
            //data_written(pdata);
            pdata->logicType = tempLogicType;
            //reset_written(pdata);
            return true;
        }
    } else if (actionCode == DIMENSION_ACTION) {
        int rows;
        int columns;
        char *columnsCharPointer = strchr(textStart,' ');
        rows = atoi(textStart);
        if(columnsCharPointer == NULL) {
            columns = atoi(textStart);
        } else {
            columns = atoi(columnsCharPointer);
        }
        if(columns > 0 && rows > 0) {
            write(pdata->socket, buffer, strlen(buffer) + 1);
            //data_written(pdata);
            pdata->rows = rows;
            pdata->columns = columns;
            //reset_written(pdata);
            return true;
        }
    } else if (actionCode == READY_ACTION) {
        int temp = atoi(textStart);
        if(temp == 2 || temp == 1) {
            write(pdata->socket, buffer, strlen(buffer) + 1);
            //data_written(pdata);
            //reset_written(pdata);
            return true;
        }
    } else {
        write(pdata->socket, buffer, strlen(buffer) + 1);
        return false;
    }
    return false;

}

bool checkIfQuit(char* buffer,DATA *pdata) {
    char *posSemi = strchr(buffer, ':');
    char *pos = strstr(posSemi + 1, endMsg);
    if (pos != NULL && pos - posSemi == 2 && *(pos + strlen(endMsg)) == '\0') {
        *(pos - 2) = '\0';
        printf("Pouzivatel %s ukoncil komunikaciu.\n", buffer);
        data_stop(pdata);
        return true;
    }
    else {
        return false;
    }
}

char* printActionQuestionByCode(ACTION_CODE actionCode) {

    if(actionCode == NUMBER_OF_ANTS_ACTION) {
        printf("\nHow many ants do you want in simulation? [write number and press enter]\n");
        return "[Number of ants]";
    } else if (actionCode == LOADING_TYPE_ACTION) {
        printf("\nHow do you want start simulation? [write number and press enter]\n");
        printf("1: All squares are white.\n");
        printf("2: Square color is random.\n");
        printf("3: Select black squares through the terminal input.\n");
        printf("4: Load dimension and squares colors from file.\n");
        printf("Q: Quit simulation\n");
        return "[Loading type]";
    } else if (actionCode == LOGIC_TYPE_ACTION) {
        printf("\nWhich logic of ants do you want? [write number and press enter]\n");
        printf("1: Direct logic.\n");
        printf("2: Inverted logic.\n");
        printf("Q: Quit simulation\n");
        return "[Logic type]";
    } else if (actionCode == DIMENSION_ACTION) {
        printf("\nSelect dimensions of ground, [write rows and columns, divided by space\n");
        return "[Dimensions]";
    } else if (actionCode == READY_ACTION) {
        printf("\nAre you ready for start? [write 1 for yes, 2 for no and press enter]\n");
        return "[READY]";
    }

    return "";
}

void writeToSocketByAction(DATA* pdata,ACTION_CODE actionCode) {
    printLog("void writeToSocketByAction(DATA* pdata,ACTION_CODE actionCode)");
    char buffer[BUFFER_LENGTH + 1];
    buffer[BUFFER_LENGTH] = '\0';
    int userNameLength = strlen(pdata->userName);
    char* action;

    action = printActionQuestionByCode(actionCode);

    fd_set inputs;
    FD_ZERO(&inputs);
    struct timeval tv;
    tv.tv_usec = 0;
    while(!data_isStopped(pdata)) {
        if(data_isWritten(pdata)) {
            break;
        }

        tv.tv_sec = 1;
        FD_SET(STDIN_FILENO, &inputs);
        select(STDIN_FILENO + 1, &inputs, NULL, NULL, &tv);
        int countCharAfterName = 2 + strlen(action);
        if (FD_ISSET(STDIN_FILENO, &inputs)) {
            sprintf(buffer, "%s%s: ", pdata->userName,action);
            char *textStart = buffer + (userNameLength + countCharAfterName);
            while (fgets(textStart, BUFFER_LENGTH - (userNameLength + countCharAfterName), stdin) > 0) {
                char *pos = strchr(textStart, '\n');
                if (pos != NULL) {
                    *pos = '\0';
                }

                if (strstr(textStart, endMsg) == textStart && strlen(textStart) == strlen(endMsg)) {
                    write(pdata->socket, buffer, strlen(buffer) + 1);
                    printf("End of communication.\n");
                    data_stop(pdata);
                } else {
                    if(writeToSocketAndSetSharedAntsData(pdata,actionCode,buffer,textStart)) {
                        data_written(pdata);
                        goto exit;
                    }

                }
            }
        }
    }
    exit:
    reset_written(pdata);
}

void makeAction(char* buffer, DATA *pdata) {
    printLog("void makeAction(char* buffer, DATA *pdata)");
    char *target = NULL;
    char *posActionStart = strchr(buffer, '[');
    char *posActionEnd;

    pthread_mutex_lock(&pdata->mutex);
    if (posActionStart != NULL) {
        //posActionStart += 1;
        posActionEnd = strchr(buffer, ']');

        target = ( char * )malloc( posActionEnd - posActionStart + 2 );
        memcpy( target, posActionStart, posActionEnd - posActionStart +1);
        target[posActionEnd - posActionStart + 1] = '\0';

        if (strcmp(target,"[Number of ants]") == 0) {
            pdata->numberOfAnts = atoi(posActionEnd + 2);
            printf("Changed number of ants: %d\n",pdata->numberOfAnts);
        }

        if (strcmp(target,"[Loading type]") == 0) {
            LOADING_TYPE loadingType = (LOADING_TYPE) atoi(posActionEnd + 2) - 1;
            pdata->loadingType = loadingType;
            printf("Changed loading type: %d\n",pdata->loadingType);
        }

        if (strcmp(target,"[Logic type]") == 0) {
            LOGIC_TYPE logicType = (LOGIC_TYPE) atoi(posActionEnd + 2) -1;
            pdata->logicType = logicType;
            printf("Changed logic type: %d\n",pdata->logicType);
        }

        if (strcmp(target,"[Dimensions]") == 0) {
            char *columnsCharPointer = strchr(posActionEnd+3,' ');
            if ( columnsCharPointer ) printf( "%s\n", columnsCharPointer );
            if(columnsCharPointer == NULL) {
                pdata->columns = atoi(posActionEnd+2);
            } else {
                pdata->columns = atoi(columnsCharPointer);
            }
            if ( posActionEnd+2 ) printf( "%s\n", posActionEnd+2 );
            pdata->rows = atoi(posActionEnd+2);

            printf("Changed rows: %d columns: %d\n",pdata->rows,pdata->columns);
        }

        if (strcmp(target,"[READY]") == 0) {
            char *columnsCharPointer = strchr(posActionEnd,' ');
            int temp = atoi(columnsCharPointer);
            if (temp == 1) {
                printf("ANTS ARE READY\n");
                pthread_cond_signal(&pdata->startGame);
            } else {
                printf("ANTS ARE NOT READY\n");
            }
        }
    }

    //if ( target ) printf( "%s\n", target );

    pthread_mutex_unlock(&pdata->mutex);
    printf("After mutex unlock\n");


    free( target );
}

void data_init(DATA *data, const char* userName, const int socket) {
    printLog("void data_init(DATA *data, const char* userName, const int socket)");
    data->socket = socket;
    data->stop = 0;
    data->written = 0;
    data->userName[USER_LENGTH] = '\0';
    data->rows = -1;
    data->columns = -1;
    data->numberOfAnts = -1;
    data->loadingType = NOT_SELECTED_LOADING_TYPE;
    data->logicType = NOT_SELECTED_ANTS_LOGIC;

    strncpy(data->userName, userName, USER_LENGTH);
    pthread_mutex_init(&data->mutex, NULL);
    pthread_mutex_init(&data->writtenMutex, NULL);
    pthread_cond_init(&data->startGame,NULL);

}

void data_destroy(DATA *data) {
    pthread_mutex_destroy(&data->mutex);
    pthread_mutex_destroy(&data->writtenMutex);
    pthread_cond_destroy(&data->startGame);
}

void data_stop(DATA *data) {
    pthread_mutex_lock(&data->mutex);
    data->stop = 1;
    printf("Stop -> 1\n");
    pthread_mutex_unlock(&data->mutex);
}

void data_written(DATA *data) {
    pthread_mutex_lock(&data->writtenMutex);
    data->written = 1;
    printf("Written -> 1\n");
    pthread_mutex_unlock(&data->writtenMutex);
}

void reset_written(DATA *data) {
    pthread_mutex_lock(&data->writtenMutex);
    data->written = 0;
    pthread_mutex_unlock(&data->writtenMutex);
}

void initSimulationSetting(DATA* pdata) {
    printLog("void initSimulationSetting(DATA* pdata)");


    if(pdata->numberOfAnts <= 0) {
        writeToSocketByAction(pdata,NUMBER_OF_ANTS_ACTION);
    }

    printLog("after: writeToSocketByAction(pdata,NUMBER_OF_ANTS_ACTION)");
    if(pdata->loadingType == NOT_SELECTED_LOADING_TYPE) {
        writeToSocketByAction(pdata,LOADING_TYPE_ACTION);
    }

    printLog("after: writeToSocketByAction(pdata,LOADING_TYPE_ACTION)");
    if(pdata->logicType == NOT_SELECTED_ANTS_LOGIC) {
        writeToSocketByAction(pdata,LOGIC_TYPE_ACTION);
    }

    printLog("after: writeToSocketByAction(pdata,LOGIC_TYPE_ACTION)");
    if(pdata->columns <= 0 || pdata->rows <= 0) {
        writeToSocketByAction(pdata,DIMENSION_ACTION);
    }

    printLog("after: writeToSocketByAction(pdata,DIMENSION_ACTION)");
    writeToSocketByAction(pdata,READY_ACTION);
}


int data_isWritten(DATA *data) {
    int written;
    pthread_mutex_lock(&data->writtenMutex);
    written = data->written;
    //printf("%d written\n",written);
    pthread_mutex_unlock(&data->writtenMutex);
    return written;
}

int data_isStopped(DATA *data) {
    int stop;
    pthread_mutex_lock(&data->mutex);
    stop = data->stop;
    //printf("%d stopped\n",stop);
    pthread_mutex_unlock(&data->mutex);
    return stop;
}

void *data_readData(void *data) {
    printLog("void *data_readData(void *data)");
    DATA *pdata = (DATA *)data;
    char buffer[BUFFER_LENGTH + 1];
    buffer[BUFFER_LENGTH] = '\0';

    while(!data_isStopped(pdata)) {
        bzero(buffer, BUFFER_LENGTH);
//        read(pdata->socket,&abc,sizeof (abc));
//        printf("%d",abc);
        if (read(pdata->socket, buffer, BUFFER_LENGTH) > 0) {
            if(!checkIfQuit(buffer,pdata)) {
                printf("%s \n", buffer);
            }
            makeAction(buffer,pdata);
            data_written(pdata);
        }
        else {
            data_stop(pdata);
        }
    }

    return NULL;
}

void *data_writeData(void *data) {
    printLog("void *data_writeData(void *data)");
    DATA *pdata = (DATA *)data;

    //pre pripad, ze chceme poslat viac dat, ako je kapacita buffra
    fcntl(STDIN_FILENO, F_SETFL, fcntl(STDIN_FILENO, F_GETFL, 0) | O_NONBLOCK);

    initSimulationSetting(pdata);
    writeMsgToAllParticipants(pdata);




    fcntl(STDIN_FILENO, F_SETFL, fcntl(STDIN_FILENO, F_GETFL, 0) & ~O_NONBLOCK);

    return NULL;
}

void printError(char *str) {
    if (errno != 0) {
        perror(str);
    }
    else {
        fprintf(stderr, "%s\n", str);
    }
    exit(EXIT_FAILURE);
}


