//
// Created by Maroš Gorný on 21. 12. 2022.
//

#include <stdlib.h>
#include <time.h>
#include "settings.h"
#include "client_server_definitions.h"
#include <string.h>


void printBackground(const BOX ***boxes, int rows,int columns) {
    printf("WHITE = 0\n");
    printf("BLACK = 1\n");
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < columns; j++) {
            printf("%d ",boxes[i][j]->color);
        }
        printf("\n");
    }
}


void printLog(char *str) {

    return;

    printf ("\033[33;1m %s \033[0m\n",str);
}


int getChanceOfBlackBox() {
    int chanceOfBlackBox;
    srand(time(NULL));
    chanceOfBlackBox = rand() % 100;
    printf("Chance of black box: %d %%\n",chanceOfBlackBox);
}

void initBoxFile(BOX* boxData, FILE* file) {
    int tempColorBox;
    if((tempColorBox = fgetc(file)) != EOF) {
        while (tempColorBox == 13 || tempColorBox == 10 || tempColorBox == 32) {
            tempColorBox = fgetc(file);
        }
    }
//    printf(" print %d",tempColorBox);
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



void shuffle(int *array, size_t n)

{
    printLogServer("void shuffle(int *array, size_t n)",1);
    if (n > 1)
    {
        size_t i;
        for (i = 0; i < n - 1; i++)
        {
            size_t j = i + rand() / (RAND_MAX / (n - i) + 1);
            int t = array[j];
            array[j] = array[i];
            array[i] = t;
        }
    }
}



