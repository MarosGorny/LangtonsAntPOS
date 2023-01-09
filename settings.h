//
// Created by Maroš Gorný on 21. 12. 2022.
//

#ifndef LANGTONSANTPOS_SETTINGS_H
#define LANGTONSANTPOS_SETTINGS_H


#include <stdio.h>
#include "structuresEnums.h"
#include <stdbool.h>


/**
* Vypise riadky a stlpce displeja[1 = cierna farba, 0 = biela farba]
*
 *@param  ***boxes BOX - 2D array pointrov na box
* @param  rows int  - Pocet riadkov
* @param  columns int  - Pocet stlpcov
* @return void
*/
void printBackground(const BOX ***boxes, int rows,int columns);

/**
* Farebny vypis textu
*
 *@param  str* char - Text ktory sa ma vytlacit
* @return void
*/
void printLog(char *str);

/**
* Vrati sancu s akou sa objavi cierne policko [0-99%]
*
* @return int - sanca v percentach
*/
int getChanceOfBlackBox();

/**
* Precita si nasledujuci znak v subore a ak to je 1 alebo 0, tak ju zapise ako farbu do boxu
*
 *@param  boxData BOX* - Box v ktorom sa bude menit farba
 *@param  file FILE* - Subor z ktoreho sa bude farba citat
* @return void
*/
void initBoxFile(BOX* boxData, FILE* file);

/**
* Nainicializuje dany box na ciernu farbu, podla sance ktoru dostane, inak na bielu farbua
*
 *@param  boxData BOX* - Box v ktorom sa bude menit farba
 *@param  chanceOfBlackBox int - Sanca pod akou sa budu tvorit cierne boxy
* @return void
*/
void initBoxRandom(BOX* boxData, int chanceOfBlackBox);

/* Arrange the N elements of ARRAY in random order.
   Only effective if N is much smaller than RAND_MAX;
   if this may not be the case, use a better random
   number generator. */
/**
* Nahodne rozhadze array
 *      funkcia je vhodna na array velkosti ktory je ovela mensi ako RAND_MAX, inak nie je velmi efektivny
*
* @return void
*/
void shuffle(int *array, size_t n);


#endif //LANGTONSANTPOS_SETTINGS_H
