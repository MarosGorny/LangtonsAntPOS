#include "client_server_definitions.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>


#include <unistd.h>
#include <fcntl.h>
#include <stdbool.h>
#include <netinet/in.h>
#include <errno.h>


void printData(DATA* pdata) {
    printLogServer("printData(DATA* pdata)");
    printf( "%s: con:%d col:%d row:%d ants:%d loaTyp%d logTyp%d\n stop:%d step:%d written:%d ready:%d \n number of clients:%d\n", pdata->userName\
                                                        ,pdata->continueSimulation\
                                                        ,pdata->columns\
                                                        ,pdata->rows\
                                                        ,pdata->numberOfAnts\
                                                        ,pdata->loadingType\
                                                        ,pdata->logicType\
                                                        ,pdata->stop\
                                                        ,pdata->step\
                                                        ,pdata->written\
                                                        ,pdata->ready\
                                                        ,pdata->numberOfClients);
}

void printLogServer(char *str) {

    printf ("\033[33;1m %s \033[0m\n",str);
//#define RESET   "\033[0m"
//#define BLACK   "\033[30m"      /* Black */
//#define RED     "\033[31m"      /* Red */
//#define GREEN   "\033[32m"      /* Green */
//#define YELLOW  "\033[33m"      /* Yellow */
//#define BLUE    "\033[34m"      /* Blue */
//#define MAGENTA "\033[35m"      /* Magenta */
//#define CYAN    "\033[36m"      /* Cyan */
//#define WHITE   "\033[37m"      /* White */
//#define BOLDBLACK   "\033[1m\033[30m"      /* Bold Black */
//#define BOLDRED     "\033[1m\033[31m"      /* Bold Red */
//#define BOLDGREEN   "\033[1m\033[32m"      /* Bold Green */
//#define BOLDYELLOW  "\033[1m\033[33m"      /* Bold Yellow */
//#define BOLDBLUE    "\033[1m\033[34m"      /* Bold Blue */
//#define BOLDMAGENTA "\033[1m\033[35m"      /* Bold Magenta */
//#define BOLDCYAN    "\033[1m\033[36m"      /* Bold Cyan */
//#define BOLDWHITE   "\033[1m\033[37m"      /* Bold White */

}

void printError(char *str) {
    if (errno != 0) {
        perror(str);
    }
    else {
        fprintf(stderr, "%s\n", str);
    }
    exit(EXIT_FAILURE);
}

void data_stop(DATA *data) {
    printLogServer("CLIENT: void data_stop(DATA *data)");
    pthread_mutex_lock(&data->mutex);
    data->stop = 1;
    printf("Stop -> 1\n");
    pthread_mutex_unlock(&data->mutex);
}

int data_isStopped(DATA *data) {
    int stop;
    pthread_mutex_lock(&data->mutex);
    stop = data->stop;
    pthread_mutex_unlock(&data->mutex);
    return stop;
}














