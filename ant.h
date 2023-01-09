//
// Created by Maroš Gorný on 16. 12. 2022.
//

#ifndef LANGTONSANTPOS_ANT_H
#define LANGTONSANTPOS_ANT_H

#include "structuresEnums.h"


/**
* Funkcie pre vlakno v ktorom sa mravec pohybuje
*
* @param  arg void* - posiela sa ant
* @return void void pointer
*/
void* antF(void* arg);

/**
* Vytlaci na terminal info o mravcovi
 * Jeho polohu,smer a farbu policka na ktorom stoji
*
* @param  ant ANT - mravec
 *@param  ***boxes BOX - 2D array pointrov na box
* @return void pointer
*/
void printAntInfo(ANT ant,const BOX ***boxes);

/**
 * Vrati farbu na ktorej aktualne stoji mravec
*
* @param  ant ANT - mravec
 *@param  ***boxes BOX - 2D array pointrov na box
* @return BACKGROUND_COLOR farba policka
*/
BACKGROUND_COLOR getBoxColorOfAnt(ANT ant,const BOX ***boxes);

/**
 * Premeni ENUM BACKGROUND_COLOR na String a vrati ho
*
* @param  bgColor BACKGROUND_COLOR - farba
* @return const char* - String
*/
const char* getBoxColorString(BACKGROUND_COLOR bgColor);

/**
 * Premeni ENUM ANT_DIRECTION na String a vrati ho
*
* @param  antDirection ANT_DIRECTION - smer
* @return const char* - String
*/
const char* getDircetionString(ANT_DIRECTION antDirection);


#endif //LANGTONSANTPOS_ANT_H
