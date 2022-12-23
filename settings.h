//
// Created by Maroš Gorný on 21. 12. 2022.
//

#ifndef LANGTONSANTPOS_SETTINGS_H
#define LANGTONSANTPOS_SETTINGS_H


#include <stdio.h>
#include "structuresEnums.h"




LOADING_TYPE setLoadingType();
LOGIC_TYPE setLogicType();
int setNumberOfAnts();
void getNumberRowsCollumns(FILE* file,int* rows,int* columns);
int getChanceOfBlackBox();
//void initBoxData(BOX* boxData, LOADING_TYPE loadingType);
void initBoxFile(BOX* boxData, FILE* file);
void initBoxRandom(BOX* boxData, int chanceOfBlackBox);
void initBoxTerminalInput(DISPLAY* display);
void chooseAntsPosition(int rows,int columns, ANT* antData);

#endif //LANGTONSANTPOS_SETTINGS_H
