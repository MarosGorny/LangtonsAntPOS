#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "ant.h"


int main(int argc,char* argv[]) {

    // split into frames from wiki
    // https://ezgif.com/split/ezgif-2-6771e98488.gif

    int columns;
    int rows;
    int areaSize;
    bool directLogic = false;

    if(argc < 2) {
        areaSize = columns = rows = 5;
    } else if (argc == 2) {
        areaSize = columns = rows = atoi(argv[1]);
    } else {
        rows =  atoi(argv[1]);
        columns = atoi(argv[2]);
    }

    printf("WIDTH:%d HEIGHT:%d\n", columns, rows);

    // Creating and initialization of display array
    BACKGROUND_COLOR display[rows][columns];
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < columns; j++) {
            display[i][j] = WHITE;
        }
    }

    printBackground((int *)display, rows, columns);

    // Creating ant
    ANT ant = {1, columns / 2, rows / 2, NORTH};

    // Printing info about ant
    printAntInfo(ant, (int *)display, columns);
    printBackground((int *)display, rows, columns);

    printf("loop\n");
    bool antIsAlive = true;
    int counter = 0;
    while (antIsAlive) {
        counter++;
        BACKGROUND_COLOR antBoxColor = getBoxColorOfAnt(ant, (int *)display, columns);
        printf("box color %d\n",antBoxColor);
        if(antBoxColor == WHITE) {
            display[ant.y][ant.x] = BLACK;
            switch (ant.direction) {
                case NORTH:
                    ant.x += directLogic ? 1:-1;
                    break;
                case EAST:
                    ant.y += directLogic ? 1:-1;
                    break;
                case SOUTH:
                    ant.x += directLogic ? -1:1;
                    break;
                case WEST:
                    ant.y += directLogic ? -1:1;
                    break;
                default:
                    fprintf(stderr,"ant[%d] direction is not set\n",ant.id);
            }
            ant.direction = directLogic ?  ((ant.direction + 1) % 4) : ((ant.direction + 3) % 4);

        } else if (antBoxColor == BLACK) {
            display[ant.y][ant.x] = WHITE;
            switch (ant.direction) {
                case NORTH:
                    ant.x += directLogic ? -1:1;
                    break;
                case EAST:
                    ant.y += directLogic ? -1:1;
                    break;
                case SOUTH:
                    ant.x += directLogic ? 1:-1;
                    break;
                case WEST:
                    ant.y += directLogic ? 1:-1;
                    break;
                default:
                    fprintf(stderr,"ant[%d] direction is not set\n",ant.id);
            }
            ant.direction = directLogic ?  ((ant.direction + 3) % 4) : ((ant.direction + 1) % 4);
        }


        if(ant.x >= columns || ant.y >= rows || ant.x < 0 || ant.y < 0) {
            printAntInfo(ant, (int *)display, columns);
            printf("Ant[%d] is dead\n",ant.id);
            printf("Iteration:%d\n",counter);
            antIsAlive = false;
        } else {
            printAntInfo(ant, (int *)display, columns);
        }
        if(counter > 1000) {
            antIsAlive = false;
            printf("Counter is maxed\n");
            printf("Iteration:%d\n",counter);
        }
    }

    printBackground((int *)display, rows, columns);

    return 0;
}

