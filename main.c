#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <pthread.h>

#include "ant.h"
#include "display.h"





void* antF(void* arg) {
    ANT* ant = arg;
    DISPLAY* antsDisplay = ant->display;

    bool antIsAlive = true;
    int counter = 0;


    printAntInfo(*ant, (const BOX ***) antsDisplay->box );
    printf("Starting positon\n");
    while (antIsAlive) {
        counter++;
        int antsX = ant->x;
        int antsY = ant->y;
        pthread_mutex_lock(antsDisplay->box[antsY][antsX]->mut);
        BACKGROUND_COLOR antBoxColor = getBoxColorOfAnt(*(ant), (const BOX ***) antsDisplay->box);
        //BACKGROUND_COLOR antBoxColor = getBoxColorOfAnt(ant, (int *)displayColors, columns);
        if(antBoxColor == WHITE) {
            printf("WHITE\n");
            antsDisplay->box[antsY][antsX]->color = BLACK;
            //displayColors[ant.y][ant.x] = BLACK;
            switch (ant->direction) {
                case NORTH:
                    ant->x += antsDisplay->directLogic ? 1:-1;
                    break;
                case EAST:
                    ant->y += antsDisplay->directLogic ? 1:-1;
                    break;
                case SOUTH:
                    ant->x += antsDisplay->directLogic ? -1:1;
                    break;
                case WEST:
                    ant->y += antsDisplay->directLogic ? -1:1;
                    break;
                default:
                    fprintf(stderr,"ant[%d] direction is not set\n",ant->id);
            }
            ant->direction = antsDisplay->directLogic ?  ((ant->direction + 1) % 4) : ((ant->direction + 3) % 4);

        } else if (antBoxColor == BLACK) {
            //printf("BLACK\n");
            antsDisplay->box[antsY][antsX]->color = WHITE;
            switch (ant->direction) {
                case NORTH:
                    ant->x += antsDisplay->directLogic ? -1:1;
                    break;
                case EAST:
                    ant->y += antsDisplay->directLogic ? -1:1;
                    break;
                case SOUTH:
                    ant->x += antsDisplay->directLogic ? 1:-1;
                    break;
                case WEST:
                    ant->y += antsDisplay->directLogic ? 1:-1;
                    break;
                default:
                    fprintf(stderr,"ant[%d] direction is not set\n",ant->id);
            }
            ant->direction = antsDisplay->directLogic ?  ((ant->direction + 3) % 4) : ((ant->direction + 1) % 4);
        }
        //printf("END LOOP\n");
        //printf("X: %d Y:%d\n",ant->x,ant->y);


        if(ant->x >= antsDisplay->width || ant->y >= antsDisplay->height || ant->x < 0 || ant->y < 0) {
            //printAntInfo(*ant, (const BOX ***) antsDisplay->box);
            printf("Ant[%d] is dead\n",ant->id);
            printf("Iteration:%d\n",counter);
            antIsAlive = false;
        } else {
            printAntInfo(*ant, (const BOX ***) antsDisplay->box );
        }
        if(counter > 1000) {
            antIsAlive = false;
            printf("Counter is maxed\n");
            printf("Iteration:%d\n",counter);
        }
        printf("UNLOCK\n");
        pthread_mutex_unlock(antsDisplay->box[antsY][antsX]->mut);
    }

}

int main(int argc,char* argv[]) {

    // split into frames from wiki
    // https://ezgif.com/split/ezgif-2-6771e98488.gif

    int columns;
    int rows;
    int areaSize;
    bool directLogic = true;

    if(argc < 2) {
        areaSize = columns = rows = 6;
    } else if (argc == 2) {
        areaSize = columns = rows = atoi(argv[1]);
    } else {
        rows =  atoi(argv[1]);
        columns = atoi(argv[2]);
    }

    printf("WIDTH:%d HEIGHT:%d\n", columns, rows);


    // Creating and initialization of displayColors array
    //BACKGROUND_COLOR displayColors[rows][columns];

    DISPLAY display = {columns, rows, directLogic=true};
    display.box = malloc(rows*sizeof (BOX**));

    for (int i = 0; i < rows; i++) {
        display.box[i] = malloc(columns*sizeof (BOX*));
        for (int j = 0; j < columns; j++) {
            //colors
            //displayColors[i][j] = WHITE;

            //boxes
            BOX* boxData = malloc(sizeof (BOX));
            display.box[i][j] = boxData;
            boxData->x = j;
            boxData->y = i;
            boxData->color = WHITE;
            pthread_mutex_t mut = PTHREAD_MUTEX_INITIALIZER;
            boxData->mut = &mut;
        }
    }

    pthread_t ants[1];
    ANT antsD[1];
    printBackground((const BOX ***) display.box, rows, columns);

    for (int i = 0; i < 1; i++) {
        antsD[i].id = i+1;
        antsD[i].x = columns /2;
        antsD[i].y = rows / 2;
        antsD[i].direction = NORTH;
        antsD[i].display = &display;

        pthread_create(&ants[i],NULL,antF,&antsD[i]);
    }
    printf("Created ant\n");


    // Creating ant
    //ANT ant = {1, columns / 2, rows / 2, NORTH};

    // Printing info about ant
    //printAntInfo(antsD[1], (int *)displayColors, columns);




    for (int i = 0; i < 1; i++) {
        pthread_join(ants[i],NULL);
    }

    printBackground((const BOX ***) display.box, rows, columns);

    /// CLEANING AND DELETING
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < columns; j++) {

            //boxes
            BOX* tempBox = display.box[i][j];
            pthread_mutex_destroy(tempBox->mut);
            free(tempBox);

        }
        free(display.box[i]);
    }
    free(display.box);



    //(int *)antsDisplay->box[antsY][antsX]->color

    return 0;
}

