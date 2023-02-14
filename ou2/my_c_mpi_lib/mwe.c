// gcc -Wall mwe.c mpi.c -o mwe

#include <stdio.h>
#include <stdlib.h>
#include "mpi.h"

void testSendRecv();
void testScatterGather();

int main(int argc, char* argv[]) {
    fprintf(stderr, "Start!\n");

    // testSendRecv();
    testScatterGather();

    fprintf(stderr, "End!\n");
    return 0;
}

void testSendRecv() {
    mpi* mpi = mpi_init(2);
    char me = mpi_getRank(mpi);
    // if (me == 1) {
    //     mpi_send(mpi, 0, (void *) "Hello parallel world!", CHAR*22);
    if (me == 0) {
        mpi_send(mpi, 1, (void *) "Hello parallel world!", CHAR*22);
    } else {
        void* msg = malloc(CHAR*22);
        mpi_receive(mpi, msg, CHAR*22);
        fprintf(stderr, "Me: %d\tReceived: %s\n", me, (char*) msg);
        free(msg);
    }
    mpi_kill(mpi, 1);
}

void testScatterGather() {
    mpi* mpi = mpi_init(3);
    char me = mpi_getRank(mpi);
    int* input;
    if (me == 0) {
        input = (int*) malloc(sizeof(int) * 3);
        scanf("%d %d %d", input, input+1, input+2);
    }
    int* data = malloc(INT);
    mpi_scatter(mpi, 0, input, data, INT, 3);
    if (me == 0) {
        free(input); 
    }
    data[0] *= 2;
    fprintf(stderr, "me: %d, data: %d\n", me, data[0]);
    int* res;
    if (me == 1) {
        res = malloc(INT * 3);
    }
    mpi_gather(mpi, 1, data, res, INT, 3);
    free(data);
    if (me == 1) {
        fprintf(stderr, "res: %d %d %d\n", res[0], res[1], res[2]);
        free(res);
    }
    mpi_kill(mpi, 1);
}
