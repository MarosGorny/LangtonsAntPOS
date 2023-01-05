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
#include <errno.h>

void printLog(char *str) {

        printf ("\033[33;1m %s \033[0m\n",str);
//#define RESET   "\033[0m"
//#define BLACK   "\033[30m"      /* Black */
//#define RED     "\033[31m"      /* Red */
//#define GREEN   "\033[32m"      /* Green */
//#define YELLOW  "\033[33m"      /* Yellow */
//#define BLUE    "\033[34m"      /* Blue */
//#define MAGENTA "\033[35m"      /* Magenta */
//#define CYAN    "\033[36m"      /* Cyan */
//#define WHITE   "\033[37m"      /* White */
//#define BOLDBLACK   "\033[1m\033[30m"      /* Bold Black */
//#define BOLDRED     "\033[1m\033[31m"      /* Bold Red */
//#define BOLDGREEN   "\033[1m\033[32m"      /* Bold Green */
//#define BOLDYELLOW  "\033[1m\033[33m"      /* Bold Yellow */
//#define BOLDBLUE    "\033[1m\033[34m"      /* Bold Blue */
//#define BOLDMAGENTA "\033[1m\033[35m"      /* Bold Magenta */
//#define BOLDCYAN    "\033[1m\033[36m"      /* Bold Cyan */
//#define BOLDWHITE   "\033[1m\033[37m"      /* Bold White */

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
        case FILE_INPUT_LOCAL:
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
        while (tempColorBox == 13 || tempColorBox == 10 || tempColorBox == 32) {
            tempColorBox = fgetc(file);
        }
    }
    printf(" print %d",tempColorBox);
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


