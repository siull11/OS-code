// gcc -Wall main.cpp mpi.cpp -o exec

#include <stdio.h>
#include <stdlib.h>
#include "mpi.h"

int main(int argc, char* argv[]) {
    fprintf(stderr, "Start!\n");

    mpi* mpi = mpi_init(2);
    char me = mpi_getRank();
    if (me == 1) {
        mpi_send(mpi, 0, (void *) "Hello parallel world!", CHAR*22);
    } else if (me == 0) {
        void* msg = mpi_receive(mpi, CHAR*22, NULL);
        fprintf(stderr, "Received: %s\n", (char*) msg);
        free(msg);
    }
    mpi_kill(mpi);

    mpi* mpi = mpi_init(3);
    char me = mpi_getRank();
    int* arr;
    if (me == 0) {
        arr = (int*) malloc(sizeof(int) * 3);
        scanf("%d %d %d", arr, arr+1, arr+2);
        // fprintf(stderr, "read: %d %d %d\n", arr[0], arr[1], arr[2]);
    }
    int* data = (int*) mpi_scatter(mpi, 0, (void *) arr, INT, 3);
    if (me == 0) {
        free(arr); 
    }
    data[0] *= 2;
    fprintf(stderr, "me: %d, data: %d\n", me, data[0]);
    int* res = (int*) mpi_gather(mpi, 1, (void *) data, INT, 3);
    free(data);
    if (me == 1) {
        fprintf(stderr, "res: %d %d %d\n", res[0], res[1], res[2]);
        free(res);
    }
    mpi_kill(mpi);


    fprintf(stderr, "End!\n");
    return 0;
}
