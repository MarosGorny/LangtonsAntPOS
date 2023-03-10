//
// Created by Maroš Gorný on 23. 12. 2022.
//

#ifndef LANGTONSANTPOS_STRUCTURESENUMS_H
#define LANGTONSANTPOS_STRUCTURESENUMS_H

#include "pthread.h"
#include <arpa/inet.h>

#define SERVER_BACKLOG 6

#define USER_LENGTH 10
#define BUFFER_LENGTH 300
extern char *endMsg;


typedef enum loadingType {
    NOT_SELECTED_LOADING_TYPE = -1,
    ALL_WHITE = 0,
    RANDOM_COLOR = 1,
    TERMINAL_INPUT = 2,
    FILE_INPUT_LOCAL = 3,
    FILE_INPUT_SERVER = 4,
}LOADING_TYPE;

typedef enum logicType {
    NOT_SELECTED_ANTS_LOGIC = -1,
    DIRECT = 0,
    INVERT = 1,
}LOGIC_TYPE;

typedef enum direction {
    NORTH = 0,
    EAST = 1,
    SOUTH = 2,
    WEST = 3,
    COLL_DIRECTION = 10,
}ANT_DIRECTION;

typedef enum backgroundColor {
    WHITE = 0,
    BLACK = 1,
}BACKGROUND_COLOR;

typedef struct sockaddr_in SA_IN;
typedef struct sockaddr SA;

typedef struct data {
    char userName[USER_LENGTH + 1];

    int numberOfAnts;
    LOADING_TYPE loadingType;
    LOGIC_TYPE logicType;
    int rows;
    int columns;

    int* sockets;
    int stop;
    int continueSimulation;
    int numberOfConnections;
    int step;
    int ready;
    int download;

    char txtFileName[100];

    int typeOfCollision;

    pthread_mutex_t mutex;
    pthread_mutex_t writtenMutex;

    pthread_cond_t startAntSimulation;
    pthread_cond_t continueAntSimulation;
    pthread_cond_t updateClients;

    int** colorOfDisplay;

} DATA;


typedef  struct box{
    BACKGROUND_COLOR color;
    int x;
    int y;
    int numberOfAnts;

    pthread_mutex_t* mut;
}BOX;

typedef enum actionCode {
    UKNOWN_ACTION = -1,
    NUMBER_OF_ANTS_ACTION = 1,
    LOADING_TYPE_ACTION = 2,
    LOGIC_TYPE_ACTION = 3,
    DIMENSION_ACTION = 4,
    SELECTING_BLACK_BOXES = 45,
    FILE_ACTION = 5,
    READY_ACTION = 6,
    WAITING_ACTION = 7,
    DOWNLOAD_ACTION = 8,
    END_ACTION = 10,
}ACTION_CODE;

typedef enum collision {
    UKNOWN_COLL = -1,
    ONLY_FIRST_ALIVE_COLL = 1,
    ALL_DIE_COLL = 2,
    HALF_DOWN_MOVEMENT_COLL = 3,
}COLLISION;

typedef struct display{
    int width;
    int height;
    int actualNumberOfAnts;

    pthread_barrier_t* barriers;
    pthread_barrier_t* mainBarrier;
    pthread_mutex_t* mut;


    LOGIC_TYPE logicType;
    BOX*** box;
    DATA* dataSocket;
    COLLISION collisionType;
}DISPLAY;

typedef struct ant {
    int id;
    int x;
    int y;
    ANT_DIRECTION direction;

    DISPLAY* display;
}ANT;


#endif //LANGTONSANTPOS_STRUCTURESENUMS_H
