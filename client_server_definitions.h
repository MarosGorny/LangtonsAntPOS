#ifndef K_DEFINITIONS_H
#define	K_DEFINITIONS_H

#include <pthread.h>
#include "structuresEnums.h"

#define USER_LENGTH 10
#define BUFFER_LENGTH 300
extern char *endMsg;

typedef struct data {
    char userName[USER_LENGTH + 1];
    pthread_mutex_t mutex;
    int socket;
    int stop;

    int columns;
    int rows;
    int numberOfAnts;

    LOADING_TYPE loadingType;
    LOGIC_TYPE logicType;
    pthread_mutex_t writtenMutex;
    int written;
} DATA;

void data_init(DATA *data, const char* userName, const int socket);
void data_destroy(DATA *data);
void data_stop(DATA *data);
int data_isStopped(DATA *data);
void *data_readData(void *data);
void *data_writeData(void *data);

void reset_written(DATA *data);
void data_written(DATA *data);
int data_isWritten(DATA *data);

void printError(char *str);



#endif //K_DEFINITIONS_H

