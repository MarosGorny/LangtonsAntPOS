//
// Created by Maros Gorny on 28. 12. 2022.
//

#include <pthread.h>
#include <stdbool.h>
#include <bits/types/FILE.h>
#include "structuresEnums.h"

#ifndef LANGTONSANTPOS_SERVER_DEFINITION_H
#define LANGTONSANTPOS_SERVER_DEFINITION_H


void writeToSocketActualData(DATA* pdata, int socket);
bool semicolonAction(char* buffer, DATA *pdata);
void *data_writeDataServer(void *data);
void *data_readDataServer(void *data);
void makeActionNew(char* buffer, DATA *pdata);
void data_destroyServer(DATA *data);
void data_initServer(DATA *data, const char* userName);

void write_file(int socket);
void send_fileServer(int socket,DATA* pdata);




#endif //LANGTONSANTPOS_SERVER_DEFINITION_H
