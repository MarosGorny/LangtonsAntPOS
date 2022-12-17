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

    while (antIsAlive) {
        counter++;
        int antsX = ant->x;
        int antsY = ant->y;
        BACKGROUND_COLOR antBoxColor = getBoxColorOfAnt(*(ant), (int *)antsDisplay->box[antsY][antsX]->color, antsDisplay->width);
        //BACKGROUND_COLOR antBoxColor = getBoxColorOfAnt(ant, (int *)displayColors, columns);
        if(antBoxColor == WHITE) {
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


        if(ant->x >= antsDisplay->width || ant->y >= antsDisplay->height || ant->x < 0 || ant->y < 0) {
            printAntInfo(*ant, (int *)antsDisplay->box[antsY][antsX]->color, antsDisplay->width);
            printf("Ant[%d] is dead\n",ant->id);
            printf("Iteration:%d\n",counter);
            antIsAlive = false;
        } else {
            printAntInfo(*ant, (int *)antsDisplay->box[antsY][antsX]->color , antsDisplay->width);
        }
        if(counter > 1000) {
            antIsAlive = false;
            printf("Counter is maxed\n");
            printf("Iteration:%d\n",counter);
        }
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
        areaSize = columns = rows = 3;
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
            BOX* boxData = malloc(sizeof (BOX*));
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

    for (int i = 0; i < 1; i++) {
        antsD[i].id = i+1;
        antsD[i].x = columns /2;
        antsD[i].y = rows / 2;
        antsD[i].direction = NORTH;
        antsD[i].display = &display;
    }


    // Creating ant
    //ANT ant = {1, columns / 2, rows / 2, NORTH};

    // Printing info about ant
    //printAntInfo(antsD[1], (int *)displayColors, columns);





//    /// CLEANING AND DELETING
//    for (int i = 0; i < rows; i++) {
//        for (int j = 0; j < columns; j++) {
//            //colors
//            displayColors[i][j] = WHITE;
//
//            //boxes
//            BOX* tempBox = display.box[i][j];
//            pthread_mutex_destroy(tempBox->mut);
//            free(tempBox);
//
//        }
//        free(display.box[i]);
//    }
//    free(display.box);

    printBackground((const BOX ***) display.box, rows, columns);
    //(int *)antsDisplay->box[antsY][antsX]->color

    return 0;
}

