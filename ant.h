//
// Created by Maroš Gorný on 16. 12. 2022.
//

#ifndef LANGTONSANTPOS_ANT_H
#define LANGTONSANTPOS_ANT_H

#include "structuresEnums.h"



void* antF(void* arg);
void printAntInfo(ANT ant,const BOX ***boxes);
BACKGROUND_COLOR getBoxColorOfAnt(ANT ant,const BOX ***boxes);
const char* getBoxColorString(BACKGROUND_COLOR bgColor);
const char* getDircetionString(ANT_DIRECTION antDirection);


#endif //LANGTONSANTPOS_ANT_H
