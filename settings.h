//
// Created by Maroš Gorný on 21. 12. 2022.
//

#ifndef LANGTONSANTPOS_SETTINGS_H
#define LANGTONSANTPOS_SETTINGS_H


#include <stdio.h>
#include "structuresEnums.h"
#include <stdbool.h>


void printLog(char *str);
char* setDimensions(char* buffer, void* data);
LOADING_TYPE setLoadingType(char* buffer,void *data);
LOGIC_TYPE setLogicType(char* buffer,void* data);
int setNumberOfAnts(char* buffer,void* data);
void getNumberRowsCollumns(FILE* file,int* rows,int* columns);
int getChanceOfBlackBox();
//void initBoxData(BOX* boxData, LOADING_TYPE loadingType);
void initBoxFile(BOX* boxData, FILE* file);
void initBoxRandom(BOX* boxData, int chanceOfBlackBox);
void initBoxTerminalInput(DISPLAY* display);
void chooseAntsPosition(int rows,int columns, ANT* antData);
bool checkIfReady(char* buffer,void *data);


#endif //LANGTONSANTPOS_SETTINGS_H
