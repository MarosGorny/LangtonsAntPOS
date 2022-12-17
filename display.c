//
// Created by Maroš Gorný on 17. 12. 2022.
//

#include "display.h"
#include <stdio.h>

//void printBackground(const int *display, int rows,int columns) {
//    //online checker https://josephpetitti.com/ant
//    printf("WHITE = 0\n");
//    printf("BLACK = 1\n");
//    for (int i = 0; i < rows; i++) {
//        for (int j = 0; j < columns; j++) {
//            printf("%d ",(*(display + (i * columns) + j)));
//        }
//        printf("\n");
//    }
//}

void printBackground(const BOX ***boxes, int rows,int columns) {
    printf("WHITE = 0\n");
    printf("BLACK = 1\n");
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < columns; j++) {
            printf("%d ",boxes[i][j]->color);
        }
        printf("\n");
    }
}
