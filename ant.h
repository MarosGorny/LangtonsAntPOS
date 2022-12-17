//
// Created by Maroš Gorný on 16. 12. 2022.
//

#ifndef LANGTONSANTPOS_ANT_H
#define LANGTONSANTPOS_ANT_H

typedef enum backgroundColor {
    WHITE = 0,
    BLACK = 1,
}BACKGROUND_COLOR;

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
}ANT;

void printAntInfo(ANT ant,const int *display, int rows);
const char* getBoxColorString(BACKGROUND_COLOR bgColor);
BACKGROUND_COLOR getBoxColorOfAnt(ANT ant,const int *display,int rows);
const char* getDircetionString(ANT_DIRECTION antDirection);
void printBackground(const int *display, int rows,int columns);

#endif //LANGTONSANTPOS_ANT_H
