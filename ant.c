//
// Created by Maroš Gorný on 16. 12. 2022.
//
#include <stdio.h>
#include "ant.h"



void printAntInfo(ANT ant,const int* display, int collumns) {
    const char* color = getBoxColorString(*(display + (ant.y * collumns) + ant.x));
    const char* direction = getDircetionString(ant.direction);
    printf("ANT[%d] = X:%03d Y:%03d COLOR:%s DIRECTION:%s\n",ant.id,ant.x,ant.y, color,direction);
}

BACKGROUND_COLOR getBoxColorOfAnt(ANT ant,const int *display, int collumns) {
   BACKGROUND_COLOR color = *(display + (ant.y * collumns) + ant.x);
}


const char* getBoxColorString(BACKGROUND_COLOR bgColor)
{
    if(bgColor == WHITE) return "WHITE";
    else if(bgColor == BLACK) return "BLACK";
    else return "BG_NOT_SET_ERR";
}

const char* getDircetionString(ANT_DIRECTION antDirection) {
    switch (antDirection) {
        case NORTH:
            return "NORTH";
        case EAST:
            return "EAST";
        case SOUTH:
            return "SOUTH";
        case WEST:
            return "WEST";
        default:
            return "DIRECTION_NOT_SET_ERR";
    }
}

void printBackground(const int *display, int rows,int columns) {
    //online checker https://josephpetitti.com/ant
    printf("WHITE = 0\n");
    printf("BLACK = 1\n");
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < columns; j++) {
            printf("%d ",(*(display + (i * columns) + j)));
        }
        printf("\n");
    }
}


