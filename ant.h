//
// Created by Maroš Gorný on 16. 12. 2022.
//

#ifndef LANGTONSANTPOS_ANT_H
#define LANGTONSANTPOS_ANT_H

typedef enum backgroundColor {
    WHITE = 0,
    BLACK = 1,
}BACKGROUND_COLOR;

typedef struct ant {
    int id;
    int x;
    int y;
}ANT;

void printAntInfo(ANT ant,const int *display, int width);
const char* getBoxColor(BACKGROUND_COLOR bgColor);

#endif //LANGTONSANTPOS_ANT_H
