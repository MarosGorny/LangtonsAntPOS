//
// Created by Maros Gorny on 28. 12. 2022.
//

#ifndef LANGTONSANTPOS_CLIENT_DEFINITION_H
#define LANGTONSANTPOS_CLIENT_DEFINITION_H

#include <pthread.h>
#include <stdbool.h>
#include <bits/types/FILE.h>
#include "structuresEnums.h"


bool writeToServer(DATA* pdata, ACTION_CODE actionCode, char* buffer, char* textStart);
void writeToSocketByAction(DATA* pdata,ACTION_CODE actionCode);
void startSendingDataToServer(DATA * pdata);
void *data_readDataClient(void *data);
void readInitData(DATA* pdata);
void data_initClient(DATA *data, const char* userName, int socket);
void *data_writeDataClient(void *data);
void processReadData(char* buffer, DATA *pdata);
void data_destroyClient(DATA *data);

void* printActionQuestionByStep(int step,DATA* pdata);
void updateAllData(DATA* pdata,char* posActionEnd);


void send_file(char* buffer, FILE *pFile, int socket,DATA* pdata);

#endif //LANGTONSANTPOS_CLIENT_DEFINITION_H
