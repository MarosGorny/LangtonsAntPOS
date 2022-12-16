#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "ant.h"


int main(int argc,char* argv[]) {

    int areaWidth;
    int areaHeight;
    int areaSize;
    bool directLogic = true;

    if(argc < 2) {
        areaSize = areaWidth = areaHeight = 11;
    } else {
        areaSize = areaWidth = areaHeight = atoi(argv[1]);
    }

    // Creating and initialization of display array
    BACKGROUND_COLOR display[areaWidth][areaHeight];
    for (int i = 0; i < areaWidth; i++) {
        for (int j = 0; j < areaHeight; j++) {
            display[i][j] = WHITE;
        }
    }

    // Creating ant
    ANT ant = {1,areaWidth/2,areaHeight/2,NORTH};

    // Printing info about ant
    printAntInfo(ant, (int *)display, areaWidth);


    for (int i = 0; i < 10; ++i) {
        if(directLogic) {
            BACKGROUND_COLOR antBoxColor = getBoxColorOfAnt(ant,(int *)display,areaWidth);
            if(antBoxColor == WHITE) {
                display[ant.x][ant.y] = BLACK;
                switch (ant.direction) {
                    case NORTH:
                        ant.x++;
                        break;
                    case EAST:
                        ant.y++;
                        break;
                    case SOUTH:
                        ant.x--;
                        break;
                    case WEST:
                        ant.y--;
                        break;
                    default:
                        fprintf(stderr,"ant[%d] direction is not set\n",ant.id);
                }
                ant.direction = (ant.direction + 1) % 4;

            } else if (antBoxColor == BLACK) {
                display[ant.x][ant.y] = WHITE;
                switch (ant.direction) {
                    case NORTH:
                        ant.x--;
                        break;
                    case EAST:
                        ant.y--;
                        break;
                    case SOUTH:
                        ant.x++;
                        break;
                    case WEST:
                        ant.y++;
                        break;
                    default:
                        fprintf(stderr,"ant[%d] direction is not set\n",ant.id);
                }
                ant.direction = (ant.direction + 3) % 4;
            }
        }
        printAntInfo(ant, (int *)display, areaWidth);
    }




    printf("Hello, World!\n");
    return 0;
}

