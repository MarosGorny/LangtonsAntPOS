//
// Created by Maros Gorny on 28. 12. 2022.
//

#include <pthread.h>
#include <stdbool.h>
#include "structuresEnums.h"

#ifndef LANGTONSANTPOS_SERVER_DEFINITION_H
#define LANGTONSANTPOS_SERVER_DEFINITION_H

bool writeToSocketAndSetSharedAntsData(DATA* pdata,ACTION_CODE actionCode, char* buffer, char* textStart);
void printData(DATA* pdata);
void *data_writeData(void *data);
void writeStateOfSharedData(DATA* pdata, int socket);
void initSimulationSetting(DATA* pdata);
void* writeMsgToAllParticipants(DATA* pdata);
int data_isStopped(DATA *data);
void writeToSocketByAction(DATA* pdata,ACTION_CODE actionCode);
void reset_written(DATA *data);
void data_written(DATA *data);
void data_stop(DATA *data);
int data_isWritten(DATA *data);
bool checkIfQuit(char* buffer,DATA *pdata);
bool checkIfPause(char* buffer,DATA *pdata);
bool checkIfContinue(char* buffer,DATA *pdata);
void makeAction(char* buffer, DATA *pdata);
void printError(char *str);
void data_destroy(DATA *data);
void data_init(DATA *data, const char* userName);
void *data_readData(void *data);

#endif //LANGTONSANTPOS_SERVER_DEFINITION_H
