//
// Created by Maros Gorny on 28. 12. 2022.
//

#ifndef LANGTONSANTPOS_CLIENT_DEFINITION_H
#define LANGTONSANTPOS_CLIENT_DEFINITION_H

#include <pthread.h>
#include <stdbool.h>
#include "structuresEnums.h"


bool writeToSocketAndSetSharedAntsData(DATA* pdata,ACTION_CODE actionCode, char* buffer, char* textStart);
void writeToSocketByAction(DATA* pdata,ACTION_CODE actionCode);
void initSimulationSetting(DATA * pdata);
void *data_readDataClient(void *data);
void readInitData(DATA* pdata);
void data_initClient(DATA *data, const char* userName, int socket);
void *data_writeDataClient(void *data);
void readMakeAction(char* buffer, DATA *pdata);
void data_destroyClient(DATA *data);

void* printActionQuestionByStep(int step,DATA* pdata);
void updateAllData(DATA* pdata,char* posActionEnd);

#endif //LANGTONSANTPOS_CLIENT_DEFINITION_H
