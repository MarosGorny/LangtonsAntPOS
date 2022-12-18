//
// Created by Maroš Gorný on 16. 12. 2022.
//
#include <stdio.h>
#include "ant.h"

void* antF(void* arg) {
    ANT* ant = arg;
    DISPLAY* antsDisplay = ant->display;

    bool antIsAlive = true;
    int counter = 0;

    printAntInfo(*ant, (const BOX ***) antsDisplay->box );
    printf("Starting positon\n");

    while (antIsAlive) {
        counter++;
        int antsOrgX = ant->x;
        int antsOrgY = ant->y;
        printf("Ant[%d] waiting for barrier org[%d]\n",ant->id,antsDisplay->actualNumberOfAnts);
        pthread_barrier_t* originalBarrier = ant->display->mainBarrier;
        pthread_barrier_wait(originalBarrier);

        //printf("Ant[%d] TryLock\n",ant->id);
        if(pthread_mutex_trylock(antsDisplay->box[antsOrgY][antsOrgX]->mut) == 0) {

            BACKGROUND_COLOR antBoxColor = getBoxColorOfAnt(*(ant), (const BOX ***) antsDisplay->box);
            if(antBoxColor == WHITE) {
                //printf("WHITE\n");
                antsDisplay->box[antsOrgY][antsOrgX]->color = BLACK;
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
                antsDisplay->box[antsOrgY][antsOrgX]->color = WHITE;
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

            if(ant->x >= antsDisplay->width || ant->y >= antsDisplay->height || ant->x < 0 || ant->y < 0) {
                printf("Ant[%d] - last position X: %d Y:%d\n",ant->id,ant->x,ant->y);
                printf("Ant[%d] is dead. After %d iterations\n",ant->id,counter);
                pthread_mutex_lock(antsDisplay->mut);
                //printf("Ant[%d] LOCKED MAIN MUT OUTSIDE\n",ant->id);
                antsDisplay->actualNumberOfAnts--;
                printf("Numbers of ants decremented by 1\n");
                antsDisplay->mainBarrier = &antsDisplay->barriers[antsDisplay->actualNumberOfAnts-1];
                pthread_mutex_unlock(antsDisplay->mut);
                //printf("Ant[%d] UNLOCKED MAIN MUT OUTSIDE\n",ant->id);
                antIsAlive = false;
            } else {
                printAntInfo(*ant, (const BOX ***) antsDisplay->box );
            }
            if(counter > 1000) {
                antIsAlive = false;
                printf("Counter is maxed\n");
                printf("Iteration:%d\n",counter);
            }
            //printf("Ant[%d] sleeping\n",ant->id);
            //printf("Ant[%d] unlock\n",ant->id);
            pthread_mutex_unlock(antsDisplay->box[antsOrgY][antsOrgX]->mut);
            //printf("After unlock\n");

        } else {
            printf("Ant[%d] has a collision on X:%d Y:%d\n",ant->id,ant->x,ant->y);
            printf("Ant[%d] is dead. After %d iterations\n",ant->id,counter);
            pthread_mutex_lock(antsDisplay->mut);
            //printf("Ant[%d] LOCKED MAIN MUT COLL\n",ant->id);
            antsDisplay->actualNumberOfAnts--;
            printf("Numbers of ants decremented by 1\n");
            antsDisplay->mainBarrier = &antsDisplay->barriers[antsDisplay->actualNumberOfAnts-1];
            pthread_mutex_unlock(antsDisplay->mut);
            //printf("Ant[%d] UNLOCKED MAIN MUT COLL\n",ant->id);
            antIsAlive = false;
        }
        printf("Ant[%d] waiting for barrier second[%d]\n",ant->id,antsDisplay->actualNumberOfAnts);
        pthread_barrier_wait(originalBarrier);
        printf("\n");

    }

}

void printAntInfo(ANT ant,const BOX ***boxes) {
    const char* color = getBoxColorString(getBoxColorOfAnt(ant,boxes));
    const char* direction = getDircetionString(ant.direction);
    printf("ANT[%d] = X:%03d Y:%03d COLOR:%s DIRECTION:%s\n",ant.id,ant.x,ant.y, color,direction);
}

BACKGROUND_COLOR getBoxColorOfAnt(ANT ant,const BOX ***boxes) {
    return boxes[ant.y][ant.x]->color;
}


const char* getBoxColorString(BACKGROUND_COLOR bgColor)
{
    if(bgColor == WHITE) return "WHITE";
    else if(bgColor == BLACK) return "BLACK";
    else return "BG_NOT_SET_ERR";
}

const char* getDircetionString(ANT_DIRECTION antDirection) {
    switch (antDirection) {
        case NORTH:
            return "NORTH";
        case EAST:
            return "EAST";
        case SOUTH:
            return "SOUTH";
        case WEST:
            return "WEST";
        default:
            return "DIRECTION_NOT_SET_ERR";
    }
}




