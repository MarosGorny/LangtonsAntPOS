#include "client_server_definitions.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <pwd.h>


void printData(DATA* pdata) {
    printLogServer("printData(DATA* pdata)",1);
    printf( "%s: con:%d col:%d row:%d ants:%d loaTyp%d logTyp%d\n stop:%d step:%d ready:%d down:%d\n number of clients:%d TXTFILE:%s\n", pdata->userName\
                                                        ,pdata->continueSimulation\
                                                        ,pdata->columns\
                                                        ,pdata->rows\
                                                        ,pdata->numberOfAnts\
                                                        ,pdata->loadingType\
                                                        ,pdata->logicType\
                                                        ,pdata->stop\
                                                        ,pdata->step\
                                                        ,pdata->ready\
                                                        ,pdata->download\
                                                        ,pdata->numberOfConnections\
                                                        ,pdata->txtFileName);
}

void printLogServer(char *str, int i) {

    //JUST FOR DEBUGG
    return;

    if(i == 0) {
        printf ("\033[33;1m %s \033[0m\n",str);
    } else if (i == 1) {
        printf ("\033[34;1m %s \033[0m\n",str); // bold blue
    } else if (i == 2) {
        printf ("\033[31;1m %s \033[0m\n",str); // bold red
    } else {
        printf ("\033[37;1m %s \033[0mm\n",str); // bold WHITE
    }

}

void printError(char *str) {
    if (errno != 0) {
        perror(str);
    }
    else {
        fprintf(stderr, "%s\n", str);
    }
    //exit(EXIT_FAILURE);
}

void data_stop(DATA *data) {
    printLogServer("CLIENT: void data_stop(DATA *data)",1);
    pthread_mutex_lock(&data->mutex);
    data->stop = 1;
    //printf("Stop -> 1\n");
    pthread_mutex_unlock(&data->mutex);
}

int data_isStopped(DATA *data) {
    //printLogServer("data_isStopped(DATA *data)",1);
    int stop;
    pthread_mutex_lock(&data->mutex);
    stop = data->stop;
    pthread_mutex_unlock(&data->mutex);
    return stop;
}

char* getPWD() {

    struct passwd pwd;
    struct passwd *result;
    char *buf;
    size_t bufsize;
    int s;
    bufsize = sysconf(_SC_GETPW_R_SIZE_MAX);
    if (bufsize == -1)
        bufsize = 0x4000; // = all zeroes with the 14th bit set (1 << 14)
    buf = malloc(bufsize);
    if (buf == NULL) {
        perror("malloc");
        exit(EXIT_FAILURE);
    }
    s = getpwuid_r(getuid(), &pwd, buf, bufsize, &result);
    free(buf);
    if (result == NULL) {
        if (s == 0)
            printf("Not found\n");
        else {
            errno = s;
            perror("getpwnam_r");
        }
        exit(EXIT_FAILURE);
    }
    char *homedir = result->pw_dir;

    return homedir;

}













