//
// Created by Maroš Gorný on 16. 12. 2022.
//

#ifndef LANGTONSANTPOS_ANT_H
#define LANGTONSANTPOS_ANT_H

#include "display.h"

typedef enum direction {
    NORTH = 0,
    EAST = 1,
    SOUTH = 2,
    WEST = 3,
}ANT_DIRECTION;

typedef struct ant {
    int id;
    int x;
    int y;
    ANT_DIRECTION direction;

    DISPLAY* display;
}ANT;


void printAntInfo(ANT ant,const BOX ***boxes);
BACKGROUND_COLOR getBoxColorOfAnt(ANT ant,const BOX ***boxes);
const char* getBoxColorString(BACKGROUND_COLOR bgColor);
const char* getDircetionString(ANT_DIRECTION antDirection);
BACKGROUND_COLOR getBoxColorOfAnt(ANT ant,const BOX ***boxes);

#endif //LANGTONSANTPOS_ANT_H
