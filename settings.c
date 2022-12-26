//
// Created by Maroš Gorný on 21. 12. 2022.
//

#include <stdlib.h>
#include <time.h>
#include "settings.h"
#include "client_server_definitions.h"
#include <fcntl.h>
#include <unistd.h>
#include <string.h>

bool checkIfReady(char* buffer,void *data) {
    DATA *pdata = (DATA *)data;
    int userNameLength = strlen(pdata->userName);

    printf("\nAre you ready for start? [write 1 for yes, 2 for no and press enter]\n");
    fd_set inputs;
    FD_ZERO(&inputs);
    struct timeval tv;
    tv.tv_usec = 0;
    while(!data_isStopped(pdata)) {
        if(data_isWritten(pdata)) {
            break;
        }


        //printf("x\n");
        tv.tv_sec = 1;
        FD_SET(STDIN_FILENO, &inputs);
        select(STDIN_FILENO + 1, &inputs, NULL, NULL, &tv);
        char* action = "[READY]";
        int countCharAfterName = 2 + strlen(action);
        if (FD_ISSET(STDIN_FILENO, &inputs)) {
            printf("y\n");
            sprintf(buffer, "%s%s: ", pdata->userName,action);
            char *textStart = buffer + (userNameLength + countCharAfterName);
            while (fgets(textStart, BUFFER_LENGTH - (userNameLength + countCharAfterName), stdin) > 0) {
                char *pos = strchr(textStart, '\n');
                if (pos != NULL) {
                    *pos = '\0';
                }
                //write(pdata->socket, buffer, strlen(buffer) + 1);

                if (strstr(textStart, endMsg) == textStart && strlen(textStart) == strlen(endMsg)) {
                    write(pdata->socket, buffer, strlen(buffer) + 1);
                    printf("Koniec komunikacie.\n");
                    data_stop(pdata);
                } else {
                    int temp = atoi(textStart);
                    if(temp == 2 || temp == 1) {

                        write(pdata->socket, buffer, strlen(buffer) + 1);

                        data_written(pdata);
                        return atoi(textStart);
                    }
                }
                printf("p\n");
            }
        }
    }





}

LOADING_TYPE setLoadingType(char* buffer,void *data) {
    DATA *pdata = (DATA *)data;
    int userNameLength = strlen(pdata->userName);
    fd_set inputs;
    FD_ZERO(&inputs);
    struct timeval tv;
    tv.tv_usec = 0;
        printf("\nHow do you want start simulation? [write number and press enter]\n");
        printf("1: All squares are white.\n");
        printf("2: Square color is random.\n");
        printf("3: Select black squares through the terminal input.\n");
        printf("4: Load dimension and squares colors from file.\n");
        printf("Q: Quit simulation\n");
        //scanf("%s",buffer);
        while(!data_isStopped(pdata)) {

            if(data_isWritten(pdata)) {
                break;
            }
            tv.tv_sec = 1;
            FD_SET(STDIN_FILENO, &inputs);
            select(STDIN_FILENO + 1, &inputs, NULL, NULL, &tv);
            char* action = "[Loading type]";
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
                        printf("Koniec komunikacie.\n");
                        data_stop(pdata);
                    } else {
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
                            return tempLoadType;
                        }
                    }
                }
            }
        }
}

LOGIC_TYPE setLogicType(char* buffer,void* data) {
    DATA *pdata = (DATA *)data;
    int userNameLength = strlen(pdata->userName);
    fd_set inputs;
    FD_ZERO(&inputs);
    struct timeval tv;
    tv.tv_usec = 0;
        printf("\nWhich logic of ants do you want? [write number and press enter]\n");
        printf("1: Direct logic.\n");
        printf("2: Inverted logic.\n");
        printf("Q: Quit simulation\n");
        //scanf("%s",buffer);

    while(!data_isStopped(pdata)) {

        if(data_isWritten(pdata)) {
            break;
        }
        tv.tv_sec = 1;
        FD_SET(STDIN_FILENO, &inputs);
        select(STDIN_FILENO + 1, &inputs, NULL, NULL, &tv);
        char* action = "[Logic type]";
        int countCharAfterName = 2 + strlen(action);
        if (FD_ISSET(STDIN_FILENO, &inputs)) {

            sprintf(buffer, "%s%s: ", pdata->userName, action);
            char *textStart = buffer + (userNameLength + countCharAfterName);
            while (fgets(textStart, BUFFER_LENGTH - (userNameLength + countCharAfterName), stdin) > 0) {
                char *pos = strchr(textStart, '\n');
                if (pos != NULL) {
                    *pos = '\0';
                }
                if (strstr(textStart, endMsg) == textStart && strlen(textStart) == strlen(endMsg)) {
                    printf("Koniec komunikacie.\n");
                    data_stop(pdata);
                } else {
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
                        return tempLogicType;
                    }
                }
            }
        }
    }

}

int setNumberOfAnts(char* buffer,void* data) {
    DATA *pdata = (DATA *)data;
    int userNameLength = strlen(pdata->userName);

    printf("\nHow many ants do you want in simulation? [write number and press enter]\n");
    fd_set inputs;
    FD_ZERO(&inputs);
    struct timeval tv;
    tv.tv_usec = 0;
    while(!data_isStopped(pdata)) {
        if(data_isWritten(pdata)) {
            break;
        }


        //printf("x\n");
        tv.tv_sec = 1;
        FD_SET(STDIN_FILENO, &inputs);
        select(STDIN_FILENO + 1, &inputs, NULL, NULL, &tv);
        char* action = "[Number of ants]";
        int countCharAfterName = 2 + strlen(action);
        if (FD_ISSET(STDIN_FILENO, &inputs)) {
            printf("y\n");
            sprintf(buffer, "%s%s: ", pdata->userName,action);
            char *textStart = buffer + (userNameLength + countCharAfterName);
            while (fgets(textStart, BUFFER_LENGTH - (userNameLength + countCharAfterName), stdin) > 0) {
                char *pos = strchr(textStart, '\n');
                if (pos != NULL) {
                    *pos = '\0';
                }
                //write(pdata->socket, buffer, strlen(buffer) + 1);

                if (strstr(textStart, endMsg) == textStart && strlen(textStart) == strlen(endMsg)) {
                    write(pdata->socket, buffer, strlen(buffer) + 1);
                    printf("Koniec komunikacie.\n");
                    data_stop(pdata);
                } else {
                    int temp = atoi(textStart);
                    if(temp > 0) {

                        write(pdata->socket, buffer, strlen(buffer) + 1);

                        data_written(pdata);
                        return atoi(textStart);
                    }
                }
                printf("p\n");
            }
        }
    }
}

char* setDimensions(char* buffer, void* data, int* rows, int* columns) {
    DATA *pdata = (DATA *)data;
    int userNameLength = strlen(pdata->userName);

    printf("\nSelect dimensions of ground, [write rows and columns, divided by space\n");
    fd_set inputs;
    FD_ZERO(&inputs);
    struct timeval tv;
    tv.tv_usec = 0;
    while(!data_isStopped(pdata)) {
        if(data_isWritten(pdata)) {
            break;
        }
        //printf("x\n");
        tv.tv_sec = 1;
        FD_SET(STDIN_FILENO, &inputs);
        select(STDIN_FILENO + 1, &inputs, NULL, NULL, &tv);
        char* action = "[Dimensions]";
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
                    printf("Koniec komunikacie.\n");
                    data_stop(pdata);
                } else {
                    char *columnsCharPointer = strchr(textStart,' ');
                    *rows = atoi(textStart);
                    if(columnsCharPointer == NULL) {
                        *columns = atoi(textStart);
                    } else {
                        *columns = atoi(columnsCharPointer);
                    }
                    if(*columns > 0 && *rows > 0) {
                        write(pdata->socket, buffer, strlen(buffer) + 1);
                        pdata->rows = *rows;
                        pdata->columns = *columns;
                        data_written(pdata);
                        return textStart;
                    }
                }
                printf("p\n");
            }
        }
    }
}

void* selectPositionsOfAnts() {
    //TODO treba spravit asi cez referenciu
    bool uncorrectSelection;
//    uncorrectSelection = true;
//    while(uncorrectSelection) {
//        for (int i = 0; i < numberOfAnts; i++) {
//            printf("\n Select postion of ant[%d] \n",i+1);
//
//            int x;
//            int y;
//            printf("X: ");
//            scanf("%s",buffer);
//
//            longTempValue = strtol(buffer, &ptr, 10);
//            x = (int)longTempValue;
//            printf("Y: ");
//            scanf("%s",buffer);
//
//            longTempValue = strtol(buffer, &ptr, 10);
//            y = (int)longTempValue;
//            display.box[x][y]->color = BLACK;
//
//        }
//
//        uncorrectSelection = false;
//    }
}

void getNumberRowsCollumns(FILE* file,int* rows,int* columns) {
    //TODO spravit aby som si mohol dat nazov a ak bude zly aby sa vypytal znova.
    if ((file = fopen("../txtFiles/test.txt","r")) == NULL){
        printf("Error! opening file");
    } else {
        fscanf(file,"%d", rows);
        fscanf(file,"%d", columns);
    }
    fclose(file);
}

int getChanceOfBlackBox() {
    int chanceOfBlackBox;
    srand(time(NULL));
    chanceOfBlackBox = rand() % 100;
    printf("Chance of black box: %d %%\n",chanceOfBlackBox);
}

void initBoxData(BOX* boxData, LOADING_TYPE loadingType) {
    switch (loadingType) {
        case ALL_WHITE:
            boxData->color = WHITE;
            break;
        case RANDOM_COLOR:
            //TODO FUNCTION TO INIT RANDOM COLORS
            break;
        case TERMINAL_INPUT:
            //TODO FUNCTION TO INIT TERMINAL INPUT
            break;
        case FILE_INPUT:
            //TODO FUNCTION TO INIT FILE INPUT
            break;
        default:
            //PRINT ERR????
            break;
    }
}

void initBoxFile(BOX* boxData, FILE* file) {
    int tempColorBox;
    if((tempColorBox = fgetc(file)) != EOF) {
        while (tempColorBox == 13 || tempColorBox == 10) {
            tempColorBox = fgetc(file);
        }
    }
    tempColorBox -= 48; //ASCII to number
    if(tempColorBox == 1) {
        boxData->color = BLACK;
    } else {
        boxData->color = WHITE;
    }
}

void initBoxRandom(BOX* boxData, int chanceOfBlackBox) {
    int randomnessBox = rand() % 100;
    if(randomnessBox < chanceOfBlackBox) {
        boxData->color = BLACK;
    } else {
        boxData->color = WHITE;
    }
}

void initBoxTerminalInput(DISPLAY* display) {
    printf("inside\n");
    char buffer[100];
    char *ptr;
    long tempValue;

    printf("To add black BOX, enter X and Y of black box\n");
    printf("If you want to quit selecting black boxes, write 'Q'\n");
    while (true) {
        int x;
        int y;
        printf("X: ");
        scanf("%s", buffer);
        if (buffer[0] == 'Q')
            break;
        //tempValue = strtol(buffer, &ptr, 10);
        tempValue = strtol(buffer, NULL, 10);
        if(buffer[0] != '0' && tempValue ==0) {
            printf("Error occured\n");
        }

        x = (int) tempValue;
        printf("Y: ");
        scanf("%s", buffer);
        if (buffer[0] == 'Q')
            break;
        tempValue = strtol(buffer, &ptr, 10);
        y = (int) tempValue;
        display->box[x][y]->color = BLACK;
    }
}

void chooseAntsPosition(int rows,int columns, ANT* antData) {
    char buffer[100];
    long tempValue = -1;

    printf("To add the ant position, enter X and Y \n");
    printf("If you want to quit program, write 'Q'\n");
    printf("Or if you want to center the ant, write 'M'\n");


    ///JUST FOR TESTING PURPOSE
    antData->x = columns/2;
    antData->y = rows/2;
    antData->direction = NORTH;
    return;


    //TODO MAKE IT BETTER
    scanf("%s", buffer);
    if (buffer[0] == 'm' || buffer[0] == 'M') {
        antData->x = columns/2;
        antData->y = rows/2;
        antData->direction = NORTH;
        return;
    }

    while (true) {
        int x;
        int y;

        while(true) {
            printf("X: ");
            scanf("%s", buffer);
            if (buffer[0] == 'Q' || buffer[0] == 'q')
                //TODO quit program
                break;

            tempValue = strtol(buffer, NULL, 10);
            if(tempValue < rows && tempValue >= 0) {
                if(buffer[0] != '0' && tempValue == 0) {
                    printf("Error occured, try again pleas\n\n");
                } else {
                    break;
                }
            } else {
                printf("Position is incorrect, try again please\n\n");
            }
        }
        x = (int) tempValue;

        while(true) {
            printf("Y: ");
            scanf("%s", buffer);
            if (buffer[0] == 'Q' || buffer[0] == 'q')
                //TODO quit program
                break;

            tempValue = strtol(buffer, NULL, 10);
            if(tempValue < columns && tempValue >= 0) {
                if(buffer[0] != '0' && tempValue == 0) {
                    printf("Error occured, try again pleas\n\n");
                } else {
                    break;
                }
            } else {
                printf("Position is incorrect, try again please\n\n");
            }
        }
        y = (int) tempValue;

        antData->x = x;
        antData->y = y;
        antData->direction = NORTH;
    }
}


