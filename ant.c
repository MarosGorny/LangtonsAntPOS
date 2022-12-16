//
// Created by Maroš Gorný on 16. 12. 2022.
//
#include <stdio.h>
#include "ant.h"



void printAntInfo(ANT ant,const int* display, int width) {
    const char* color = getBoxColor(*(display+(ant.x*width) + ant.y));
    printf("ANT[%d] = X:%03d Y:%03d COLOR:%s\n",ant.id,ant.x,ant.y, color);
}


const char* getBoxColor(BACKGROUND_COLOR bgColor)
{
    if(bgColor == WHITE) return "WHITE";
    else if(bgColor == BLACK) return "BLACK";
    else return "NOT_SET_ERR";
}

