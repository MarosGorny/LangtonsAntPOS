//
// Created by Maroš Gorný on 16. 12. 2022.
//
#include <stdio.h>
#include "ant.h"



void printAntInfo(ANT ant,const int* display, int width) {
    const char* color = getBoxColorString(*(display + (ant.x * width) + ant.y));
    printf("ANT[%d] = X:%03d Y:%03d COLOR:%s\n",ant.id,ant.x,ant.y, color);
}

BACKGROUND_COLOR getBoxColorOfAnt(ANT ant,const int *display, int width) {
   BACKGROUND_COLOR color = *(display + (ant.x * width) + ant.y);
}


const char* getBoxColorString(BACKGROUND_COLOR bgColor)
{
    if(bgColor == WHITE) return "WHITE";
    else if(bgColor == BLACK) return "BLACK";
    else return "NOT_SET_ERR";
}

void printBackground(const int *display, int width,int height) {
    for (int i = 0; i < height; i++) {
        for (int j = 0; j < width; ++j) {
            printf("%d ",(*(display + (j * height) + i)));
        }
        printf("\n");
    }
}


