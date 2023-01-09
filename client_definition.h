//
// Created by Maros Gorny on 28. 12. 2022.
//

#ifndef LANGTONSANTPOS_CLIENT_DEFINITION_H
#define LANGTONSANTPOS_CLIENT_DEFINITION_H

#include <pthread.h>
#include <stdbool.h>
#include <bits/types/FILE.h>
#include "structuresEnums.h"

/**
* Posle write na server podla danej akcie
*
* @param  pdata DATA*  - Zdielane data
* @param  actionCode ACTION_CODE - Kod akcie ktora sa ma vykonat
* @param  buffer char* - Buffer spravy
* @param  textStart char*  - Pointer na zaciatok spravy
* @return bool
*/
bool writeToServer(DATA* pdata, ACTION_CODE actionCode, char* buffer, char* textStart);

/**
* Skontroluje ci v buffri nie je nahodou akcia na ukoncenie, zisti akciu a zavola funkciu writeToServer
*
* @param  pdata DATA*  - Zdielane data
* @param  actionCode ACTION_CODE - Kod akcie ktora sa ma vykonat
* @return void
*/
void writeToSocketByAction(DATA* pdata,ACTION_CODE actionCode);

/**
* Skontroluje aktualny krok a podla toho zavola writeToSocketByAction s prislisnou akciou
*
* @param  pdata DATA*  - Zdielane data
* @return void
*/
void startSendingDataToServer(DATA * pdata);

/**
* Funkcia pre vlakno, v ktorej sa citaju spravy zo servera
*
* @param  pdata DATA*  - Zdielane data
* @return void*
*/
void *data_readDataClient(void *data);

/**
* Precita data zo serveru a podla nich updatne zdielane data u klienta
*
* @param  pdata DATA*  - Zdielane data
* @return void
*/
void readInitData(DATA* pdata);

/**
* Inicializuje klienta na startovacie hodnoty
*
* @param  pdata DATA*  - Zdielane data
* @param  userName const char*  - Meno klienta
* @param  socket int  - Socket servera
* @return void
*/
void data_initClient(DATA *data, const char* userName, int socket);

/**
* Funkcia pre vlakno, v ktorej sa budu posielat spravy na server
*
* @param  data* void  - Zdielane data
* @return void*
*/
void *data_writeDataClient(void *data);

/**
* Funkcia v ktorej sa podla aktualneho kroku a dat v buffri nastavia hodnoty
 * Vacsinou sa pouziva ak iny klient odoslal novsie data,
 * server nam potom posle aktualne a tato funkcia ich aktualizuje
*
* @param  buffer *char  - Buffer textu
* @param  data* void  - Zdielane data
* @return void*
*/
void processReadData(char* buffer, DATA *pdata);

/**
* Funkcia na upratanie dat a uvolnenie pamati
*
* @param  data* void  - Zdielane data
* @return void
*/
void data_destroyClient(DATA *data);

/**
* Podla daneho kroku sa vypise otazka pre klienta ze ako chce dalej pokracovat.
*
* @param  step int  - Aktualny krok
* @param  data* void  - Zdielane data
* @return void
*/
void* printActionQuestionByStep(int step,DATA* pdata);

/**
* Rozparsuje buffer a vsetky rozparsovane data aktualizuje do zdielanych dat
*
* @param  data* void  - Zdielane data
* @param  posActionEnd char*  - Pointer kde v buffri konci akcia (teda koniec hranatej zatvorky "]")
* @return void
*/
void updateAllData(DATA* pdata,char* posActionEnd);

/**
* Funkcia po riadkoch odosle dany txt file serveru.
*
* @param  buffer char*  - Buffer s textom
* @param  pFile* FILE  - Subor na odoslanie
* @param  pdata DATA*  - Zdielane data
* @return void
*/
void send_file(char* buffer, FILE *pFile,DATA* pdata);

#endif //LANGTONSANTPOS_CLIENT_DEFINITION_H
