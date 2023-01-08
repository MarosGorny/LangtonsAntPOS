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
#include <errno.h>

char* endMsg = ":end";
char* pauseMsg = ":pause";
char* continueMsg = ":continue";
char* quitMsg = ":quit";


//void send_file(FILE *pFile, int i);

/**
 * Inicializacia zdielanych dat u klienta.
*
* @param  data DATA* - zdielane data
* @param  userName char* - meno servera
* @param  socket int - socket servera
* @return void
*/
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
    //data->condStartListeningArray == NULL;
}

/**
 * Vlakno na posielanie dat pre server.
*
* @param  data DATA* - zdielane data
* @return void
*/
void *data_writeDataClient(void *data) {
    printLogServer("CLIENT: void *data_writeData(void *data)",1);
    DATA *pdata = (DATA *)data;

    //pre pripad, ze chceme poslat viac dat, ako je kapacita buffra
    fcntl(STDIN_FILENO, F_SETFL, fcntl(STDIN_FILENO, F_GETFL, 0) | O_NONBLOCK);
    printData(pdata);

    startSendingDataToServer(pdata);


    fcntl(STDIN_FILENO, F_SETFL, fcntl(STDIN_FILENO, F_GETFL, 0) & ~O_NONBLOCK);
    printf("RETURNNNN data_writeDataClient\n");
    return NULL;
}

/**
 * Precitanie prvej spravy od servera a nasledne updatnutie zdielanych dat.
*
* @param  data DATA* - zdielane data
* @return void
*/
void readInitData(DATA* pdata) {
    printLogServer("readInitData(DATA* pdata)",1);

    char buffer[BUFFER_LENGTH + 1];
    buffer[BUFFER_LENGTH] = '\0';
    bzero(buffer, BUFFER_LENGTH);

    if (read(pdata->sockets[0], buffer, BUFFER_LENGTH) > 0) {
        //printf("BUFF----%s\n",buffer);

        char *posActionEnd;
        posActionEnd = strchr(buffer, ']');

        updateAllData(pdata,posActionEnd);
        printData(pdata);
        printf("Changed Shared date state\n");

    }
    else {
        printf("ELSE STOP\n");
        data_stop(pdata);
    }

    printActionQuestionByStep(pdata->step,NULL);

}

/**
 * Vlakno na prijimanie dat od servera.
*
* @param  data DATA* - zdielane data
* @return void
*/
void *data_readDataClient(void *data) {
    printLogServer("CLIENT: void *data_readData(void *data)",1);
    
    DATA *pdata = (DATA *)data;

    char buffer[BUFFER_LENGTH + 1];
    buffer[BUFFER_LENGTH] = '\0';
    while(!data_isStopped(pdata)) {
        bzero(buffer, BUFFER_LENGTH);

        //citanie dat od servera
        if (read(pdata->sockets[0], buffer, BUFFER_LENGTH) > 0) {
            printf("READ DATA: %s\n",buffer);


            processReadData(buffer, pdata);

            pthread_mutex_lock(&pdata->mutex);
            //printf("data_readData: INCREMENTIG STEP\n");
            printf("STEP: %d\n",pdata->step);
            printActionQuestionByStep(pdata->step,pdata);
            pthread_mutex_unlock(&pdata->mutex);
        }
        else {
            printf("ELSE STOP\n");
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

    printf("STEP ON READMAKEACTION START: %d\n",step);

    printf("BUFFER:%s\n",buffer);



    //String pre zaciatok a koniec zatvoriek (akcie)
    char *posActionBracketsStart = strchr(buffer, '[');
    char *posActionBracketsEnd = strchr(buffer, ']');

    if(step == 10) {
        //time and filename

        time_t t = time(NULL);
        struct tm tm = *localtime(&t);
        char fileNameString[50];
        sprintf(fileNameString, "%s/savedFileClient_%04d%02d%02d_%02d%02d.txt",getPWD(),tm.tm_year+1900, tm.tm_mday, tm.tm_mon + 1, tm.tm_hour, tm.tm_min);

        char oldName[50];
        sprintf(oldName,"%s/tempLocal.txt",getPWD());
        printf("oldName: %s\n",oldName);
        rename(oldName,fileNameString);
        printf("File is saved on server with name:%s\n",fileNameString);
        printf("RENAMED\n");
    }

    if(step == 9) {
        //String pre zaciatok poslanej informacie

        if(pdata->download == 2) {
            char *target = NULL;

            printf("PDATA STEP 9 READING FROM FILE\n");


            target = (char *) malloc(posActionBracketsEnd - posActionBracketsStart + 2);
            memcpy(target, posActionBracketsStart, posActionBracketsEnd - posActionBracketsStart + 1);
            target[posActionBracketsEnd - posActionBracketsStart + 1] = '\0';

            printf("Bracket Action: %s\n", target);

            FILE *fptr;
            //fptr = fopen("../txtFiles/writedFile-.txt","w");

            if ((fptr = fopen("/home/gorny/tempLocal.txt","a")) == NULL){
                printf("Error! opening file");
            }
            if(fptr == NULL) {
                printf("Error writing\n");
            } else {
                printf("writed:%s\n",posActionBracketsEnd+3);
                printf("%s\n",posActionBracketsEnd+3);
                if(strcmp(posActionBracketsEnd+3,"END") == 0) {
                    pdata->step++;
                    pthread_cond_signal(&pdata->continueAntSimulation);
                } else {
                    fprintf(fptr,"%s", posActionBracketsEnd + 3);
                }

            }
            printf("CLOSING FILE\n");
            fclose(fptr);
            printf("CLOSED FILE\n");
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
                    printf("TXT FILENAME IS %s\n",pdata->txtFileName);
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

/**
 * Rozdeli aky typ dat(akcie) sa ma poslat serveru podla toho v akom kroku aplikacii klient je.
 *
* @param  data DATA* - zdielane data
* @return void
*/
void startSendingDataToServer(DATA* pdata) {
    printLog("void startSendingDataToServer(DATA* pdata)");
    printf("STEP %d\n",pdata->step);

    while(!data_isStopped(pdata)) {
        //printf("DATA IS STOPPED?:%d\n",pdata->stop);
        //if(pdata->numberOfAnts <= 0) {
        pthread_mutex_lock(&pdata->mutex);
        int step = pdata->step;
        pthread_mutex_unlock(&pdata->mutex);

        //printf(" startSendingDataToServer STEP: %d",pdata->step);
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
                usleep(200);
                break;
            case 10:
                writeToSocketByAction(pdata,END_ACTION);
                default:
                break;
        }
    }
}


/**
 * Vypise pre klienta na terminal info ku danemu kroku.
 * 
* @param  step int - Aktualny krok v aplikacii
* @param  data DATA* - zdielane data
* @return void
*/
void* printActionQuestionByStep(int step, DATA* pdata) {
    printLogServer("char* printActionQuestionByStep(int step)",1);
    printf("STEP: %d\n",step);
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
        printf("Q: Quit simulation\n");
        //return "[Loading type]";
    } else if (step == 3) {
        printf("\nWhich logic of ants do you want? [write number and press enter]\n");
        printf("1: Direct logic.\n");
        printf("2: Inverted logic.\n");
        printf("Q: Quit simulation\n");
        //return "[Logic type]";
    } else if (step == 4) {

        if(pdata->rows <= 0) {
            printf("\nSelect dimensions of ground, [write rows and columns, divided by space]\n");
        } else {
            printf("\nSelect black boxes, [write X and Y, divided by space]\n");
            printf("\tWrite \"OK\" to continue\n");
        }

        //return "[Dimensions]";
    } else if (step == 5) {
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

/**
 * Poslu sa informacie pre server podla toho aka akcia bola.
 *
* @param  step int - Aktualny krok v aplikacii
* @param  data DATA* - zdielane data
* @return void
*/
void writeToSocketByAction(DATA* pdata,ACTION_CODE actionCode) {
    printLogServer("void writeToSocketByAction(DATA* pdata,ACTION_CODE actionCode)",1);
    printf("ACTION CODE IN writeToSocketByAction : %d\n",actionCode);
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
        printf("action = getActionStringInBracket(SELECTING_BLACK_BOXES);\n");
        action = getActionStringInBracket(SELECTING_BLACK_BOXES);
    } else {
        //ak sa svet nacitava zo suboru
            action = getActionStringInBracket(actionCode);
            printf("action = getActionStringInBracket(actionCode);\n");
    }

//////
    if(tempStep == 5 ) {
        if(strcmp(pdata->txtFileName,"NULL") == 0) {
            action = "[FILENAME]";
        } else {
            //TODO TADADA
            if(pdata->loadingType==FILE_INPUT_LOCAL) {
                action = "[FileL]";
                printf("action = FileL;\n");
                //Poslanie dat serveru
                writeToServer(pdata, FILE_ACTION, buffer, NULL);
            printf("GOTO EXIT FILE INPUT_LOCAL\n");
            goto exit;
            } else if (pdata->loadingType==FILE_INPUT_SERVER) {
                action = "[FileS]";
                printf("action = FileS;\n");
            }
        }

    }
//////


    fd_set inputs;
    FD_ZERO(&inputs);
    struct timeval tv;
    tv.tv_usec = 0;



    printf("BEFORE WHILE\n");
    while(!data_isStopped(pdata)) {
//        printf("INSIDE WHILE\n");


        tv.tv_sec = 1;
        FD_SET(STDIN_FILENO, &inputs);
        select(STDIN_FILENO + 1, &inputs, NULL, NULL, &tv);

        //!!!!!!!!!!!!!!!!!!!! predtym to bolo pred --tv.tv_sec = 1;
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
                    printf("Written msg before simulation: %s\n",buffer);
                    write(pdata->sockets[0], buffer, strlen(buffer) + 1);

                    printf("End of communication.\n");
                    data_stop(pdata);
                    goto exit;

                } else {
                    printf("BUFFER TO WRITE: %s  (writeToSocketByAction)\n",buffer);
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
    printf("BUFFER IN SOCKET %s\n",buffer);
    printf("SOCKEEEEEEEEEEEET %d\n",pdata->sockets[0]);
    printf("Action code: %d\n",actionCode);
    if(actionCode == NUMBER_OF_ANTS_ACTION) {
        int temp = atoi(textStart);
        if(temp > 0) {

            write(pdata->sockets[0], buffer, strlen(buffer) + 1);
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
            write(pdata->sockets[0], buffer, strlen(buffer) + 1);
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
            write(pdata->sockets[0], buffer, strlen(buffer) + 1);
            //data_written(pdata);
            pdata->rows = rows;
            pdata->columns = columns;
            if(pdata->loadingType == TERMINAL_INPUT) {
                printf("SELECT POSTION OF BLACK BOXES[write: x y]\n");
            }

            //reset_written(pdata);
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
                printf("POSTIONS SENDED x:%d, y:%d\n",x,y);
                return true;
            } else {
                printf("WRONG POSITIONS\n");
                printf("POSTIONS x:%d, y:%d\n",x,y);
            }
        }
    } else if (actionCode == FILE_ACTION) {

        if(strcmp(pdata->txtFileName,"NULL") == 0) {
            strcpy(pdata->txtFileName,textStart);

            write(pdata->sockets[0], buffer, strlen(buffer) + 1);
            return true;
        } else {
            printf("FILE ACTION\n");
            if(pdata->loadingType == FILE_INPUT_LOCAL) {
                printf("FILE ACTION LOCAL\n");
                FILE *fptrRead;
                printf("else if (actionCode == FILE_ACTION)\n");
                printf("HOMEDIR :%s\n",getPWD());

                char fileNameString[50];
                sprintf(fileNameString, "%s/%s", getPWD(),pdata->txtFileName);
                printf("FILENAME OPEN STRING: %s\n",fileNameString);

                if ((fptrRead = fopen(fileNameString,"r")) == NULL){
                    printf("Error! opening file");
                } else {
                    printf("SENDING FILE\n");
                    send_file(buffer, fptrRead, pdata->sockets[0],pdata);
                    printf("SENDED FILE\n");
                    //pdata->step++;
                    return true;
                }
            } else if (pdata->loadingType == FILE_INPUT_SERVER) {
                printf("FILE ACTION SERVER\n");

                //TODO poslat serveru meno
                return true;
            }
        }




    } else if (actionCode == READY_ACTION) {
        printf("IN READY\n");
        int temp = atoi(textStart);
        printf("AFTER ATOI\n");
        if(temp == 2 || temp == 1) {
            write(pdata->sockets[0], buffer, strlen(buffer) + 1);
            //data_written(pdata);
            //reset_written(pdata);
            return true;
        }
    } else if (actionCode == WAITING_ACTION) {
        write(pdata->sockets[0], buffer, strlen(buffer) + 1);
        printLog("INSIDE WAITING ACTION - CLIENT");
        return true;
    } else if (actionCode == DOWNLOAD_ACTION) {
        printLog("INSIDE DOWNLOAD ACTION - CLIENT");
        int temp = atoi(textStart);
        if(temp >= 1 && temp <= 3) {
            write(pdata->sockets[0], buffer, strlen(buffer) + 1);
            return true;
        }
    } else if (actionCode == END_ACTION) {
        int temp = atoi(textStart);
        if(temp == 1 || temp == 2) {
            write(pdata->sockets[0], buffer, strlen(buffer) + 1);
            return true;
        }

    } else {
        write(pdata->sockets[0], buffer, strlen(buffer) + 1);
        printLog("INSIDE ELSE WRITE TO SOCKET - CLIENT");
        return false;
    }
    return false;

}

void send_file(char* buffer, FILE *pFile, int socket,DATA* pdata) {
    printLog("void send_file(FILE *pFile, int socket)");
    char* action = "[FileL]";
    int userNameLength = strlen(pdata->userName);
    int countCharAfterName = 2 + strlen(action);
    sprintf(buffer, "%s%s: ", pdata->userName,action);
    char *textStart = buffer + (userNameLength + countCharAfterName);
    int n = 0;
    while (fgets(textStart, BUFFER_LENGTH - (userNameLength + countCharAfterName), pFile) != NULL)  {
        printf("FILE TO WRITE: %s\n",buffer);
        usleep(500);
        if(write(pdata->sockets[0], buffer, strlen(buffer) + 1) < 0) {
            printf("ERRRRRRRRRRR\n");
        } else {
            printf("Successfull\n");
        }
    }
    //sleep(6);


    printLog("OUT OF: void send_file(FILE *pFile, int socket)");
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

/**
 * Updatnutie zdielanych dat na zaklade stringu ktory bol prijaty
*
* @param  data DATA* - zdielane data
* @param  posActionEnd char* - string od konca hranatej zatvorky v ktorej bola akcia
* @return void
*/
void updateAllData(DATA* pdata,char* posActionEnd) {
    printLogServer("void updateAllData(DATA* pdata,char* posActionEnd)",1);
    printf("UPDATE ALL DATA START: %s\n",posActionEnd+2);
    printf("1\n");
    char* token = strtok(posActionEnd+2," ");
    printf("2\n");
    int tempNumber;
    char* txtFile;

    if(token != NULL) {
        printf("3\n");
        // loop through the string to extract all other tokens
        for (int i = 0; i < 11; i++) {
            if(i != 10) {
                tempNumber = atoi(token);
            }
//            else {
//                strcpy(txtFile,token);
//            }

            //printf("SWITCH i=%d temp=%d\n",i,tempNumber);
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
                    //pdata->txtFileName = txtFile;
                    break;
                default:
                    break;
            }
            token = strtok(NULL, " ");

        }

    }
    printLogServer("END:void updateAllData(DATA* pdata,char* posActionEnd)",0);
}
