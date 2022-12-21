//
// Created by Maroš Gorný on 21. 12. 2022.
//

#ifndef LANGTONSANTPOS_SETTINGS_H
#define LANGTONSANTPOS_SETTINGS_H

#include <stdbool.h>
#include <stdio.h>
#include "display.h"

typedef enum loadingType {
    NOT_SELECTED_LOADING_TYPE = -1,
    ALL_WHITE = 0,
    RANDOM_COLOR = 1,
    TERMINAL_INPUT = 2,
    FILE_INPUT = 3,
}LOADING_TYPE;

typedef enum logicType {
    NOT_SELECTED_ANTS_LOGIC = -1,
    DIRECT = 0,
    INVERT = 1,
}LOGIC_TYPE;

LOADING_TYPE setLoadingType();
LOGIC_TYPE setLogicType();
int setNumberOfAnts();
void getNumberRowsCollumns(FILE* file,int* rows,int* columns);
int getChanceOfBlackBox();
void initBoxData(BOX* boxData, LOADING_TYPE loadingType);

#endif //LANGTONSANTPOS_SETTINGS_H
