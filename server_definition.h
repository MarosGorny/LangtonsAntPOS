//
// Created by Maros Gorny on 28. 12. 2022.
//

#include <pthread.h>
#include <stdbool.h>
#include <bits/types/FILE.h>
#include "structuresEnums.h"

#ifndef LANGTONSANTPOS_SERVER_DEFINITION_H
#define LANGTONSANTPOS_SERVER_DEFINITION_H

/**
 * Posle vsetky dolezite udaje z DATA* do socketu.
*
* @param  data DATA* - zdielane data
 * @param  socket int - socket kam budu poslane data
* @return void void pointer
*/
void writeToSocketActualData(DATA* pdata, int socket);

/**
 * Nastavi zdielane data, v pripade, ze ide o akciu s dvojbodkou [:pause/:continue/:end]
*
* @param  buffer char* - buffer s textom
* @param  data DATA* - zdielane data
* @return bool - ci sa jednalo o akciu s dvojbodkou, alebo nie
*/
bool semicolonAction(char* buffer, DATA *pdata);

/**
 * Funkcia pre vlakno, v ktorej sa budu odosielat data klientovi
*
* @param  data* void - zdielane data
* @return void*
*/
void *data_writeDataServer(void *data);

/**
 * Funkcia pre vlakno, v ktorej sa budu citat data od klientov
*
* @param  data* void - zdielane data
* @return void*
*/
void *data_readDataServer(void *data);

/**
 * Funkcia precita buffer a podla akcie bude pokracovat dalej
 *      (akcia je vzdy v hranatych zatvorkach - napr [Dimensions]
 *
* @param  buffer char* - zdielane data
* @param  data* void - zdielane data
* @return void
*/
void makeActionNew(char* buffer, DATA *pdata);

/**
* Funkcia na upratanie dat a uvolnenie pamati
*
* @param  data* void  - Zdielane data
* @return void
*/
void data_destroyServer(DATA *data);

/**
* Inicializuje server na startovacie hodnoty
*
* @param  pdata DATA*  - Zdielane data
* @param  userName const char*  - Meno servera
* @return void
*/
void data_initServer(DATA *data, const char* userName);


/**
* Po riadkoch posle klientovi dany textovy subor
*
* @param  socket int - socket klienta
* @param  pdata DATA*  - Zdielane data
* @return void
*/
void send_fileServer(int socket,DATA* pdata);





#endif //LANGTONSANTPOS_SERVER_DEFINITION_H
