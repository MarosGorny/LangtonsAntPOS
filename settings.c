//
// Created by Maroš Gorný on 21. 12. 2022.
//

#include <stdlib.h>
#include <time.h>
#include <stdbool.h>
#include "settings.h"

LOADING_TYPE setLoadingType() {
    char buffer[100];

    while(true) {
        printf("\nHow do you want start simulation? [write number and press enter]\n");
        printf("1: All squares are white.\n");
        printf("2: Square color is random.\n");
        printf("3: Select black squares through the terminal input.\n");
        printf("4: Load dimension and squares colors from file.\n");
        printf("Q: Quit simulation\n");
        scanf("%s",buffer);

        switch (buffer[0]) {
            case '1':
                printf("All squares are white.\n");
                return ALL_WHITE;
            case'2':
                printf("Square color is random.\n");
                return RANDOM_COLOR;
            case'3':
                printf("Select black square through the terminal input.\n");
                return TERMINAL_INPUT;
            case'4':
                printf("Load dimension and squares color from file.\n");
                return FILE_INPUT;
            case'Q':
            case'q':
                printf("Closing simulation...\n");
                return NOT_SELECTED_LOADING_TYPE;
            default:
                printf("Selection is unknown. Try again please.\n");
                break;
        }
    }
}

LOGIC_TYPE setLogicType() {
    char buffer[100];
    while(true) {
        printf("\nWhich logic of ants do you want? [write number and press enter]\n");
        printf("1: Direct logic.\n");
        printf("2: Inverted logic.\n");
        printf("Q: Quit simulation\n");
        scanf("%s",buffer);

        switch (buffer[0]) {
            case '1':
                printf("Logic of ants is direct.\n");
                return DIRECT;
            case '2':
                printf("Logic of ants is inverted.\n");
                return INVERT;
            case'q':
            case'Q':
                printf("Closing simulation...\n");
                return NOT_SELECTED_ANTS_LOGIC;
            default:
                printf("Selection is unknown. Try again please.\n");
                break;
        }
    }
}

int setNumberOfAnts() {
    char buffer[100];
    char* ptr;
    int tempValue =-1;
    while(true) {
        //TODO Treba dorobit pripad, ked sa zada vacsie mnozstvo alebo sa zada pismeno
        printf("\nHow many ants do you want in simulation? [write number and press enter]\n");
        scanf("%s",buffer);
        tempValue = (int) strtol(buffer, &ptr, 10);
        if(tempValue > 0) {
            return tempValue;
        } else {
            printf("Number of ants must be greater than 0\n");
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

