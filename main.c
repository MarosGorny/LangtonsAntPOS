#include <stdio.h>
#include <stdlib.h>
#include "ant.h"


int main(int argc,char* argv[]) {

    int areaWidth;
    int areaHeight;
    int areaSize;

    if(argc < 2) {
        areaSize = areaWidth = areaHeight = 11;
    } else {
        areaSize = areaWidth = areaHeight = atoi(argv[1]);
    }

    BACKGROUND_COLOR display[areaWidth][areaHeight];
    for (int i = 0; i < areaWidth; i++) {
        for (int j = 0; j < areaHeight; j++) {
            display[i][j] = WHITE;
        }
    }

    ANT ant = {1,areaWidth/2,areaHeight/2};

    printAntInfo(ant, (int *)display, areaWidth);

    printf("Hello, World!\n");
    return 0;
}

