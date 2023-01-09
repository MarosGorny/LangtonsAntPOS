#ifndef K_DEFINITIONS_H
#define	K_DEFINITIONS_H

#include <pthread.h>
#include <stdbool.h>
#include "structuresEnums.h"

/**
* Vypisu sa vsetky atributy zdielanych dat
*
* @param  pdata DATA*  - Zdielane data
* @return void
*/
void printData(DATA* pdata);

/**
* Vypise error alebo posle ho to sderr
*
* @param  str char*  - Text v pripade chyby
* @return void
*/
void printError(char *str);

/**
* Vypis stringu v roznych farbach, podla nastaveneho parametra i
*
* @param  str char*  - Text ktory treba vypisat
* @param  i int  - Farba[1/2/3]
* @return void
*/
void printLogServer(char *str,int i);

/**
* Skontroluje ci atribut stop v zdielanych datach je zapnuty alebo vypnuty
*
* @param  pdata DATA*  - Zdielane data
* @return int - hodnota atributu stop
*/
int data_isStopped(DATA *data);

/**
* Nastavi atribut stop v zdielanych dat na zapnuty (1)
*
* @param  pdata DATA*  - Zdielane data
* @return void
*/
void data_stop(DATA *data);

/**
* Vrati miesto akutualneho adresa z ktoreho sa bude citat subor v danom vlakne
*
* @return char* - cesta k aktualnemu adresaru
*/
char* getPWD();

#endif //K_DEFINITIONS_H

