#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <pthread.h>
#include <time.h>

#include "ant.h"
#include "display.h"





int main(int argc,char* argv[]) {


    // split into frames from wiki
    // https://ezgif.com/split/ezgif-2-6771e98488.gif

    int columns;
    int rows;

    bool directLogic = false;
    bool randomBlackBoxes = true;
    const int numberOfAnts = 1;

    if(argc < 2) {
        columns = rows = 5;
    } else if (argc == 2) {
        columns = rows = atoi(argv[1]);
    } else {
        rows =  atoi(argv[1]);
        columns = atoi(argv[2]);
    }

    printf("WIDTH:%d HEIGHT:%d\n", columns, rows);
    pthread_mutex_t mainMut = PTHREAD_MUTEX_INITIALIZER;


    //Creating barrier
    pthread_barrier_t barriers[numberOfAnts];
    for (int i = 0; i < numberOfAnts; i++) {
        pthread_barrier_init(&barriers[i], NULL, i+1);
    }


    //Creating display
    DISPLAY display = {columns, rows, numberOfAnts, barriers,&barriers[numberOfAnts-1], &mainMut, directLogic=directLogic};

    //Creating 2D dynamic array of boxes , pointer of pointers
    display.box = malloc(rows*sizeof (BOX**));

    //Creating mutexes
    pthread_mutex_t boxMutexes[rows][columns];

    //Randomness of black boxes
    srand(time(NULL));
    int randomnessGeneral = rand() % 100;
    if(randomBlackBoxes) {
        printf("Chance of black box: %d %%\n",randomnessGeneral);
    }


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

            if(randomBlackBoxes) {
                int randomnessBox = rand() % 100;
                if(randomnessBox < randomnessGeneral) {
                    boxData->color = BLACK;
                } else {
                    boxData->color = WHITE;
                }
            } else {
                boxData->color = WHITE;
            }



            //Mutex initialization and assignation to boxData
            boxMutexes[i][j] = (pthread_mutex_t)PTHREAD_MUTEX_INITIALIZER;
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
        if(i == 1) {
            antsD[i].direction = EAST;
            antsD[i].x = 2;
            antsD[i].y = 0;
        } else if(i == 0){
            antsD[i].direction = NORTH;
            antsD[i].x = columns /2;
            antsD[i].y = rows / 2;
        } else {
            antsD[i].direction = EAST;
            antsD[i].x = 0;
            antsD[i].y = 0;
        }
        antsD[i].display = &display;

        pthread_create(&ants[i],NULL,antF,&antsD[i]);
        printf("Created ant[%d]\n",i+1);
    }

    void *counter = 0;
    int counterOfFinishedAnts = 0;
    printf("1111111111111111111\n");

    for (int i = 0; i < numberOfAnts; i++) {
        //printf("22222222222222222\n");
        pthread_join(ants[i],&counter);
        counterOfFinishedAnts += *((int*)counter);
        free(counter);
    }
    //printf("333333333333333333333333333333333\n");
    printf("FINISHED ANTS %d\n",counterOfFinishedAnts);

    //Prints background
    printBackground((const BOX ***) display.box, rows, columns);

    //time and filename
    time_t t = time(NULL);
    struct tm tm = *localtime(&t);
    char fileNameString[50];
    sprintf(fileNameString, "../txtFiles/writedFile_%02dhod_%02dmin.txt", tm.tm_mday, tm.tm_mon + 1, tm.tm_hour, tm.tm_min);


    //Writing to file
    FILE *fptr;

    //fptr = fopen("../txtFiles/writedFile-.txt","w");
    fptr = fopen(fileNameString,"w");

    if(fptr == NULL) {
        printf("Error writing\n");
    } else {
        for (int i = 0; i < rows; i++) {
            for (int j = 0; j < columns; j++) {
                //Get tempBox
                if (display.box[i][j]->color == WHITE) {
                    fprintf(fptr,"0");
                } else if (display.box[i][j]->color == BLACK) {
                    fprintf(fptr,"1");
                } else {
                    fprintf(fptr,"X");
                };
            }
            fprintf(fptr,"\n");
        }
    }

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

    //pthread_barrier_destroy(barriers);
    pthread_mutex_destroy(&mainMut);

    //Destroying barrier
    for (int i = 0; i < numberOfAnts; i++) {
        pthread_barrier_destroy(&barriers[i]);
    }



    return 0;
}

