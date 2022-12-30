//
// Created by Maros Gorny on 28. 12. 2022.
//

#ifndef LANGTONSANTPOS_CLIENT_DEFINITION_H
#define LANGTONSANTPOS_CLIENT_DEFINITION_H

#include <pthread.h>
#include <stdbool.h>
#include "structuresEnums.h"

int data_isWritten(DATA *data);
bool writeToSocketAndSetSharedAntsData(DATA* pdata,ACTION_CODE actionCode, char* buffer, char* textStart);
void reset_written(DATA *data);
void writeToSocketByAction(DATA* pdata,ACTION_CODE actionCode);
void initSimulationSetting(DATA * pdata);
void *data_readData(void *data);
void readInitData(DATA* pdata);
void data_initClient(DATA *data, const char* userName, int socket);
void *data_writeDataClient(void *data);
void printData(DATA* pdata);
void *data_readDataClient(void *data);
int data_isStopped(DATA *data);
void makeAction(char* buffer, DATA *pdata);
void data_written(DATA *data);
void data_stop(DATA *data);
bool checkIfQuit(char* buffer,DATA *pdata);
bool checkIfPause(char* buffer,DATA *pdata);
bool checkIfContinue(char* buffer,DATA *pdata);
void data_destroy(DATA *data);
void printError(char *str);
char* printActionQuestionByStep(int step);
int getDataByAction(char* posActionEnd, int action);
void updateAllData(DATA* pdata,char* posActionEnd);
bool semicolonAction(char* buffer, DATA *pdata);

#endif //LANGTONSANTPOS_CLIENT_DEFINITION_H
