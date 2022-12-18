//
// Created by Maroš Gorný on 17. 12. 2022.
//

#ifndef LANGTONSANTPOS_DISPLAY_H
#define LANGTONSANTPOS_DISPLAY_H

#include <stdbool.h>
#include "pthread.h"

typedef enum backgroundColor {
    WHITE = 0,
    BLACK = 1,
}BACKGROUND_COLOR;

typedef struct box{
    BACKGROUND_COLOR color;
    int x;
    int y;

    pthread_mutex_t* mut;
}BOX;

typedef struct display{
    int width;
    int height;
    int actualNumberOfAnts;

    pthread_barrier_t* barriers;
    pthread_barrier_t* mainBarrier;
    pthread_mutex_t* mut;

    bool directLogic;
    BOX*** box;
}DISPLAY;

void printBackground(const BOX ***boxes, int rows,int columns);

#endif //LANGTONSANTPOS_DISPLAY_H
