//
// Created by Maros Gorny on 28. 12. 2022.
//

#include <pthread.h>
#include <stdbool.h>
#include "structuresEnums.h"

#ifndef LANGTONSANTPOS_SERVER_DEFINITION_H
#define LANGTONSANTPOS_SERVER_DEFINITION_H


void writeStateOfSharedData(DATA* pdata, int socket);
bool semicolonAction(char* buffer, DATA *pdata);
void *data_writeDataServer(void *data);
void *data_readDataServer(void *data);
void makeActionNew(char* buffer, DATA *pdata);
void data_destroyServer(DATA *data);
void data_init(DATA *data, const char* userName);

void write_file(int socket);




#endif //LANGTONSANTPOS_SERVER_DEFINITION_H
