#ifndef K_DEFINITIONS_H
#define	K_DEFINITIONS_H

#include <pthread.h>
#include <stdbool.h>
#include "structuresEnums.h"

void printData(DATA* pdata);
void printError(char *str);
void printLogServer(char *str);
int data_isStopped(DATA *data);
void data_stop(DATA *data);

#endif //K_DEFINITIONS_H

