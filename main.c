#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <pthread.h>

#include "ant.h"
#include "display.h"


int main(int argc,char* argv[]) {

    // split into frames from wiki
    // https://ezgif.com/split/ezgif-2-6771e98488.gif

    int columns;
    int rows;
    //int areaSize;

    bool directLogic = true;
    const int numberOfAnts = 1;

    if(argc < 2) {
        columns = rows = 3;
    } else if (argc == 2) {
        columns = rows = atoi(argv[1]);
    } else {
        rows =  atoi(argv[1]);
        columns = atoi(argv[2]);
    }

    printf("WIDTH:%d HEIGHT:%d\n", columns, rows);



    //Creating display
    DISPLAY display = {columns, rows, directLogic=directLogic};
    //Creating 2D dynamic array of boxes , pointer of pointers
    display.box = malloc(rows*sizeof (BOX**));

    //Creating mutexes
    pthread_mutex_t boxMutexes[rows][columns];

    //Initialization of boxes and creating
    for (int i = 0; i < rows; i++) {
        //Creating ROWS of boxes
        display.box[i] = malloc(columns*sizeof (BOX*));
        for (int j = 0; j < columns; j++) {

            //***BOXES***
            //Creating box
            BOX* boxData = malloc(sizeof (BOX));
            //Assigning box to position [i][j]
            display.box[i][j] = boxData;

            //Initialization of box
            boxData->x = j;
            boxData->y = i;
            boxData->color = WHITE;

            //Mutex initialization and assignation to boxData
            boxMutexes[i][j] = PTHREAD_MUTEX_INITIALIZER;
            boxData->mut = &boxMutexes[i][j];
        }
    }

    //pthreads of ants
    pthread_t ants[numberOfAnts];
    //data of ants
    ANT antsD[numberOfAnts];

    //Prints background
    printBackground((const BOX ***) display.box, rows, columns);

    for (int i = 0; i < numberOfAnts; i++) {
        antsD[i].id = i+1;
        antsD[i].x = columns /2;
        antsD[i].y = rows / 2;
        antsD[i].direction = NORTH;
        antsD[i].display = &display;

        pthread_create(&ants[i],NULL,antF,&antsD[i]);
        printf("Created ant[%d]\n",i+1);
    }

    for (int i = 0; i < numberOfAnts; i++) {
        pthread_join(ants[i],NULL);
    }

    //Prints background
    printBackground((const BOX ***) display.box, rows, columns);

    /// CLEANING AND DELETING
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < columns; j++) {
            //Get tempBox
            BOX* tempBox = display.box[i][j];
            //Destroy mutex
            pthread_mutex_destroy(tempBox->mut);
            //Destroy box
            free(tempBox);
        }
        //Destroy rows
        free(display.box[i]);
    }
    //Destroy whole 2Darray
    free(display.box);

    return 0;
}

