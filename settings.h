//
// Created by Maroš Gorný on 21. 12. 2022.
//

#ifndef LANGTONSANTPOS_SETTINGS_H
#define LANGTONSANTPOS_SETTINGS_H


#include <stdio.h>
#include "structuresEnums.h"
#include <stdbool.h>


void printBackground(const BOX ***boxes, int rows,int columns);
void printLog(char *str);
int getChanceOfBlackBox();
void initBoxFile(BOX* boxData, FILE* file);
void initBoxRandom(BOX* boxData, int chanceOfBlackBox);

/* Arrange the N elements of ARRAY in random order.
   Only effective if N is much smaller than RAND_MAX;
   if this may not be the case, use a better random
   number generator. */
void shuffle(int *array, size_t n);


#endif //LANGTONSANTPOS_SETTINGS_H
