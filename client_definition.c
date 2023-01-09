//
// Created by Maros Gorny on 28. 12. 2022.
//

#include "client_definition.h"
#include "client_server_definitions.h"
#include "settings.h"
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <malloc.h>
#include <stdlib.h>


char* endMsg = ":end";
char* pauseMsg = ":pause";
char* continueMsg = ":continue";
char* quitMsg = ":quit";



void data_initClient(DATA* data, const char* userName, int socket) {
    printLogServer("CLIENT: void data_initClient(DATA *data, const char* userName,int socket)",1);

    data->userName[USER_LENGTH] = '\0';
    strncpy(data->userName, userName, USER_LENGTH);

    data->numberOfAnts = 0;
    data->loadingType = NOT_SELECTED_LOADING_TYPE;
    data->logicType = NOT_SELECTED_ANTS_LOGIC;
    data->rows = 0;
    data->columns = 0;

    data->sockets = (int *) calloc(1, sizeof(int));
    data->sockets[0] = socket;
    data->stop = 0;
    data->continueSimulation = 1;
    data->numberOfConnections = 1;
    data->step = 0;
    data->ready = 0;
    data->download = 0;

    memset(data->txtFileName, 0, sizeof data->txtFileName);
    data->txtFileName[99] = '\0';
    strncpy(data->txtFileName, "NULL", USER_LENGTH);


    pthread_mutex_init(&data->mutex, NULL);
    pthread_mutex_init(&data->writtenMutex, NULL);

    //conds init
    pthread_cond_init(&data->startAntSimulation, NULL);
    pthread_cond_init(&data->continueAntSimulation, NULL);
    pthread_cond_init(&data->updateClients, NULL);

}


void *data_writeDataClient(void *data) {
    printLogServer("CLIENT: void *data_writeData(void *data)",1);
    DATA *pdata = (DATA *)data;

    //pre pripad, ze chceme poslat viac dat, ako je kapacita buffra
    fcntl(STDIN_FILENO, F_SETFL, fcntl(STDIN_FILENO, F_GETFL, 0) | O_NONBLOCK);

    startSendingDataToServer(pdata);


    fcntl(STDIN_FILENO, F_SETFL, fcntl(STDIN_FILENO, F_GETFL, 0) & ~O_NONBLOCK);
    return NULL;
}


void readInitData(DATA* pdata) {
    printLogServer("readInitData(DATA* pdata)",1);

    char buffer[BUFFER_LENGTH + 1];
    buffer[BUFFER_LENGTH] = '\0';
    bzero(buffer, BUFFER_LENGTH);

    if (read(pdata->sockets[0], buffer, BUFFER_LENGTH) > 0) {

        char *posActionEnd;
        posActionEnd = strchr(buffer, ']');

        updateAllData(pdata,posActionEnd);
    }
    else {
        data_stop(pdata);
    }

    printActionQuestionByStep(pdata->step,NULL);

}


void *data_readDataClient(void *data) {
    printLogServer("CLIENT: void *data_readData(void *data)",1);
    
    DATA *pdata = (DATA *)data;

    char buffer[BUFFER_LENGTH + 1];
    buffer[BUFFER_LENGTH] = '\0';
    while(!data_isStopped(pdata)) {
        bzero(buffer, BUFFER_LENGTH);

        //citanie dat od servera
        if (read(pdata->sockets[0], buffer, BUFFER_LENGTH) > 0) {

            processReadData(buffer, pdata);

            printActionQuestionByStep(pdata->step,pdata);

        }
        else {
            data_stop(pdata);
        }

    }

    return NULL;
}

void processReadData(char* buffer, DATA *pdata) {
    printLogServer("void processReadData(char* buffer, DATA *pdata)",1);

    pthread_mutex_lock(&pdata->mutex);
    int step = pdata->step;
    pthread_mutex_unlock(&pdata->mutex);


    //String pre zaciatok a koniec zatvoriek (akcie)
    char *posActionBracketsStart = strchr(buffer, '[');
    char *posActionBracketsEnd = strchr(buffer, ']');

    if(step == 10 && pdata->download == 1) {
        //time and filename

        time_t t = time(NULL);
        struct tm tm = *localtime(&t);
        char fileNameString[50];
        sprintf(fileNameString, "%s/savedFileClient_%04d%02d%02d_%02d%02d.txt",getPWD(),tm.tm_year+1900, tm.tm_mday, tm.tm_mon + 1, tm.tm_hour, tm.tm_min);

        char oldName[50];
        sprintf(oldName,"%s/tempLocal.txt",getPWD());
        rename(oldName,fileNameString);
        printf("File is saved on server with name:%s\n",fileNameString);
    }

    if(step == 9) {
        //String pre zaciatok poslanej informacie

        if(pdata->download == 2) {
            char *target = NULL;

            target = (char *) malloc(posActionBracketsEnd - posActionBracketsStart + 2);
            memcpy(target, posActionBracketsStart, posActionBracketsEnd - posActionBracketsStart + 1);
            target[posActionBracketsEnd - posActionBracketsStart + 1] = '\0';

            FILE *fptr;

            if ((fptr = fopen("/home/gorny/tempLocal.txt","a")) == NULL){
                printf("Error! opening file");
            }
            if(fptr == NULL) {
                printf("Error writing\n");
            } else {
                if(strcmp(posActionBracketsEnd+3,"END") == 0) {
                    pthread_mutex_lock(&pdata->mutex);
                    pdata->step++;
                    pthread_mutex_unlock(&pdata->mutex);
                    pthread_cond_signal(&pdata->continueAntSimulation);

                } else {
                    fprintf(fptr,"%s", posActionBracketsEnd + 3);
                }

            }
            fclose(fptr);

            free( target );
        }


    } else if (posActionBracketsStart != NULL) {
        printLogServer("INSIDE if (posActionBracketsStart != NULL)",2);


        updateAllData(pdata, posActionBracketsEnd);

        pthread_mutex_lock(&pdata->mutex);
        switch (step) {
            case 0:
                printf("Changed Shared date state\n");
                break;
            case 1:
                printf("Changed number of ants: %d\n",pdata->numberOfAnts);
                break;
            case 2:
                printf("Changed loading type: %d\n",pdata->loadingType+1);
                break;
            case 3:
                printf("Changed logic type: %d\n",pdata->logicType+1);
                break;
            case 4:
                printf("Changed rows: %d columns: %d\n",pdata->rows,pdata->columns);
                break;
            case 5:
                {
                    if(strcmp(pdata->txtFileName,"NULL") == 0) {

                    }
                    printf("file name is:%s\n",pdata->txtFileName);
                }
                break;
            case 6:
                {
                    if (pdata->ready == 1) {
                        printf("Simulation is ready\n");
                        pthread_cond_signal(&pdata->startAntSimulation);
                    } else {
                        printf("OK, let's wait\n");
                    }
                }
                break;
            default:
                break;
        }
        pthread_mutex_unlock(&pdata->mutex);


    }
    printLogServer("END:void processReadData(char* buffer, DATA *pdata)",0);

}


void startSendingDataToServer(DATA* pdata) {
    printLog("void startSendingDataToServer(DATA* pdata)");

    while(!data_isStopped(pdata)) {

        pthread_mutex_lock(&pdata->mutex);
        int step = pdata->step;
        pthread_mutex_unlock(&pdata->mutex);

        switch (step) {
            case 1:
                writeToSocketByAction(pdata,NUMBER_OF_ANTS_ACTION);
                break;
            case 2:
                writeToSocketByAction(pdata,LOADING_TYPE_ACTION);
                break;
            case 3:
                writeToSocketByAction(pdata,LOGIC_TYPE_ACTION);
                break;
            case 4:
                writeToSocketByAction(pdata,DIMENSION_ACTION);
                break;
            case 5:
                writeToSocketByAction(pdata,FILE_ACTION);
                break;
            case 6:
                writeToSocketByAction(pdata,READY_ACTION);
                break;
            case 7:
                writeToSocketByAction(pdata,WAITING_ACTION);
                break;
            case 8:
                writeToSocketByAction(pdata,DOWNLOAD_ACTION);
                break;
            case 9:
                pdata->step++;
                printActionQuestionByStep(pdata->step,pdata);
                sleep(1);
                break;
            case 10:
                writeToSocketByAction(pdata,END_ACTION);
                default:
                break;
        }
    }
}



void* printActionQuestionByStep(int step, DATA* pdata) {
    printLogServer("char* printActionQuestionByStep(int step)",1);

    if(step == 1) {
        printf("\nHow many ants do you want in simulation? [write number and press enter]\n");
        //return "[Number of ants]";
    } else if (step == 2) {
        printf("\nHow do you want start simulation? [write number and press enter]\n");
        printf("1: All squares are white.\n");
        printf("2: Square color is random.\n");
        printf("3: Select black squares through the terminal input.\n");
        printf("4: Load dimension and squares colors from LOCAL file.\n");
        printf("5: Load dimension and squares colors from SERVER file.\n");
        printf("[\":end\" for quit]\n");
        //return "[Loading type]";
    } else if (step == 3) {
        printf("\nWhich logic of ants do you want? [write number and press enter]\n");
        printf("1: Direct logic.\n");
        printf("2: Inverted logic.\n");
        printf("[\":end\" for quit]\n");
        //return "[Logic type]";
    } else if (step == 4) {

        pthread_mutex_lock(&pdata->mutex);

        if(pdata->rows <= 0) {
            printf("\nSelect dimensions of ground, [write rows and columns, divided by space]\n");
            printf("[\":end\" for quit]\n");
        } else {
            printf("\nSelect black boxes, [write X and Y, divided by space]\n");
            printf("\tWrite \"OK\" to continue\n");
        }
        pthread_mutex_unlock(&pdata->mutex);

        //return "[Dimensions]";
    } else if (step == 5) {
        pthread_mutex_lock(&pdata->mutex);
        if(strcmp(pdata->txtFileName,"NULL") == 0) {

            if(pdata->loadingType == FILE_INPUT_LOCAL) {
                printf("File must be in: %s",getPWD());
                printf("\nWrite name of the file on your computer\n");
            } else if (pdata->loadingType == FILE_INPUT_SERVER){
                printf("File must be in: %s",getPWD());
                printf("\nWrite name of the file on server\n");
            }
        } else {
            printf("\nFile %s was loaded\n",pdata->txtFileName);
        }
        pthread_mutex_unlock(&pdata->mutex);

    } else if (step == 6) {
        printf("\nAre you ready to start? [write 1 for yes and press enter]\n");
        //return "[READY]";
    } else if (step == 7) {
        printf("\nWaiting for simulation\n");
    } else if (step == 8) {
        printf("\nWhere do you want to download finished world? [write number and press enter]\n");
        printf("1: Server.\n");
        printf("2: Clients.\n");
        printf("3: Nowhere.\n");
        printf("[\":end\" for quit]\n");
    } else if (step == 10) {
        printf("\nPlease, select action? [write number and press enter]\n");
        printf("1: Start new simulation.\n");
        printf("2: End simulation.\n");
    }
    
    return NULL;
}

char* getActionStringInBracket(ACTION_CODE actionCode) {

    switch (actionCode) {
        case NUMBER_OF_ANTS_ACTION:
            return "[Number of ants]";
        case LOADING_TYPE_ACTION:
            return "[Loading type]";
        case LOGIC_TYPE_ACTION:
            return "[Logic type]";
        case DIMENSION_ACTION:
            return "[Dimensions]";
        case SELECTING_BLACK_BOXES:
            return "[SELECTING BLACK BOXES]";
        case READY_ACTION:
            return "[READY]";
        case WAITING_ACTION:
            return "";
        case DOWNLOAD_ACTION:
            return "[DOWNLOAD]";
        case END_ACTION:
            return "[END]";
        default:
            return "";
    }
}


void writeToSocketByAction(DATA* pdata,ACTION_CODE actionCode) {
    printLogServer("void writeToSocketByAction(DATA* pdata,ACTION_CODE actionCode)",1);
    char buffer[BUFFER_LENGTH + 1];
    buffer[BUFFER_LENGTH] = '\0';
    int userNameLength = strlen(pdata->userName);
    char* action;

    pthread_mutex_lock(&pdata->mutex);
    int tempStep = pdata->step;
    int rows = pdata->rows;
    pthread_mutex_unlock(&pdata->mutex);

    //ak je krok 4 (vyberanie rozmeru), ale rozmer uz bol vybraty
    if(tempStep == 4 && rows != 0) {
        action = getActionStringInBracket(SELECTING_BLACK_BOXES);
    } else {
        //ak sa svet nacitava zo suboru
            action = getActionStringInBracket(actionCode);
    }


    if(tempStep == 5 ) {
        if(strcmp(pdata->txtFileName,"NULL") == 0) {
            action = "[FILENAME]";
        } else {
            if(pdata->loadingType==FILE_INPUT_LOCAL) {
                //Poslanie dat serveru
                writeToServer(pdata, FILE_ACTION, buffer, NULL);
            goto exit;
            } else if (pdata->loadingType==FILE_INPUT_SERVER) {
                action = "[FileS]";
            }
        }

    }

    fd_set inputs;
    FD_ZERO(&inputs);
    struct timeval tv;
    tv.tv_usec = 0;


    while(!data_isStopped(pdata)) {


        tv.tv_sec = 1;
        FD_SET(STDIN_FILENO, &inputs);
        select(STDIN_FILENO + 1, &inputs, NULL, NULL, &tv);

        if(pdata->step != tempStep) {
            break;
        }

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
                    write(pdata->sockets[0], buffer, strlen(buffer) + 1);
                    printf("End of communication.\n");
                    data_stop(pdata);
                    goto exit;

                } else {
                    if(tempStep == 4 && pdata->rows != 0) {
                        if(writeToServer(pdata, SELECTING_BLACK_BOXES, buffer, textStart)) {
                            goto exit;
                        }
                    } else {
                        if(writeToServer(pdata, actionCode, buffer, textStart)) {
                            goto exit;
                        }
                    }

                }
            }
        }
    }
    exit: ;
}



bool writeToServer(DATA* pdata, ACTION_CODE actionCode, char* buffer, char* textStart) {
    printLog("writeToServer(DATA* pdata,ACTION_CODE actionCode, char* buffer, char* textStart)");
    if(actionCode == NUMBER_OF_ANTS_ACTION) {
        int temp = atoi(textStart);
        if(temp > 0) {

            write(pdata->sockets[0], buffer, strlen(buffer) + 1);
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
                printf("Load dimension and squares color from local file.\n");
                tempLoadType = FILE_INPUT_LOCAL;
                break;
            case'5':
                printf("Load dimension and squares color from server file.\n");
                tempLoadType = FILE_INPUT_LOCAL;
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
            write(pdata->sockets[0], buffer, strlen(buffer) + 1);
            pdata->loadingType = tempLoadType;
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
            write(pdata->sockets[0], buffer, strlen(buffer) + 1);
            pdata->logicType = tempLogicType;
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
            write(pdata->sockets[0], buffer, strlen(buffer) + 1);
            pdata->rows = rows;
            pdata->columns = columns;
            if(pdata->loadingType == TERMINAL_INPUT) {
                printf("Select positionf of black box[write: x y]\n");
            }

            return true;
        }
    } else if(actionCode == SELECTING_BLACK_BOXES) {
        if(strstr(textStart, "OK")) {
            write(pdata->sockets[0], buffer, strlen(buffer) + 1);
        } else {
            int x;
            int y;
            char *yPointer = strchr(textStart,' ');
            x = atoi(textStart);
            y = atoi(yPointer);

            if((x >= 0 && x < pdata->rows) && (y >= 0 && y < pdata->columns)) {
                write(pdata->sockets[0], buffer, strlen(buffer) + 1);
                printf("Position sent x:%d, y:%d\n",x,y);
                return true;
            } else {
                printf("Wrong position entered, please try again\n");
            }
        }
    } else if (actionCode == FILE_ACTION) {

        if(strcmp(pdata->txtFileName,"NULL") == 0) {
            strcpy(pdata->txtFileName,textStart);

            write(pdata->sockets[0], buffer, strlen(buffer) + 1);
            return true;
        } else {
            if(pdata->loadingType == FILE_INPUT_LOCAL) {

                FILE *fptrRead;

                char fileNameString[50];
                sprintf(fileNameString, "%s/%s", getPWD(),pdata->txtFileName);

                if ((fptrRead = fopen(fileNameString,"r")) == NULL){
                    printf("Error! opening file");
                } else {
                    send_file(buffer, fptrRead, pdata->sockets[0],pdata);
                    return true;
                }
            } else if (pdata->loadingType == FILE_INPUT_SERVER) {
                printf("FILE ACTION SERVER\n");

                //TODO poslat serveru meno
                return true;
            }
        }




    } else if (actionCode == READY_ACTION) {
        int temp = atoi(textStart);
        if(temp == 2 || temp == 1) {
            write(pdata->sockets[0], buffer, strlen(buffer) + 1);
            return true;
        }
    } else if (actionCode == WAITING_ACTION) {
        write(pdata->sockets[0], buffer, strlen(buffer) + 1);
        return true;
    } else if (actionCode == DOWNLOAD_ACTION) {
        int temp = atoi(textStart);
        if(temp >= 1 && temp <= 3) {
            write(pdata->sockets[0], buffer, strlen(buffer) + 1);
            return true;
        }
    } else if (actionCode == END_ACTION) {
        int temp = atoi(textStart);
        if(temp == 1 || temp == 2) {
            write(pdata->sockets[0], buffer, strlen(buffer) + 1);
            if(temp == 2) {
                data_stop(pdata);
            }
            return true;
        }

    } else {
        write(pdata->sockets[0], buffer, strlen(buffer) + 1);
        return false;
    }
    return false;

}

void send_file(char* buffer, FILE *pFile,DATA* pdata) {
    printLog("void send_file(FILE *pFile, int socket)");
    char* action = "[FileL]";
    int userNameLength = strlen(pdata->userName);
    int countCharAfterName = 2 + strlen(action);
    sprintf(buffer, "%s%s: ", pdata->userName,action);
    char *textStart = buffer + (userNameLength + countCharAfterName);
    while (fgets(textStart, BUFFER_LENGTH - (userNameLength + countCharAfterName), pFile) != NULL)  {
        printf("FILE TO WRITE: %s\n",buffer);
        usleep(500);
        if(write(pdata->sockets[0], buffer, strlen(buffer) + 1) < 0) {
            printf("Sending was unsuccessful\n");
        }
    }
    pdata->step++;
}




void data_destroyClient(DATA *data) {
    pthread_mutex_destroy(&data->mutex);
    pthread_mutex_destroy(&data->writtenMutex);

    pthread_cond_destroy(&data->startAntSimulation);
    pthread_cond_destroy(&data->continueAntSimulation);
    pthread_cond_destroy(&data->updateClients);

    free(data->sockets);
}


void updateAllData(DATA* pdata,char* posActionEnd) {
    printLogServer("void updateAllData(DATA* pdata,char* posActionEnd)",1);
    char* token = strtok(posActionEnd+2," ");
    int tempNumber;

    if(token != NULL) {
        // loop through the string to extract all other tokens
        for (int i = 0; i < 11; i++) {
            if(i != 10) {
                tempNumber = atoi(token);
            }

            switch (i) {
                case 0:
                    pdata->numberOfAnts = tempNumber;
                    break;
                case 1:
                    pdata->loadingType = tempNumber;
                    break;
                case 2:
                    pdata->logicType = tempNumber;
                    break;
                case 3:
                    pdata->rows = tempNumber;
                    break;
                case 4:
                    pdata->columns = tempNumber;
                    break;
                case 5:
                    pdata->stop = tempNumber;
                    break;
                case 6:
                    pdata->continueSimulation = tempNumber;
                    break;
                case 7:
                    pdata->step = tempNumber;
                    break;
                case 8:
                    pdata->ready = tempNumber;
                    break;
                case 9:
                    pdata->download = tempNumber;
                    break;
                case 10:
                    strcpy(pdata->txtFileName,token);
                    break;
                default:
                    break;
            }
            token = strtok(NULL, " ");
        }
    }
}
