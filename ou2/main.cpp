// gcc -Wall main.cpp mpi.cpp -o exec

#include <stdio.h>
#include "mpi.h"

int main(int argc, char* argv[]) {
    fprintf(stderr, "Start!\n");
    mpi* mpi = init(3);
    int me = getMe();
    // if (me == 1) {
    //     send(mpi, 0, (void *) "Hello parallel world!", CHAR, 22);
    // } else if (me == 0) {
    //     void* msg = receive(mpi, CHAR, 22);
    //     fprintf(stderr, "Received: %s\n", (char*) msg);
    //     free(msg);
    // }
    int* arr;

    if (me == 0) {
        arr = (int*) malloc(sizeof(int) * 3);
        scanf("%d %d %d", arr, arr+1, arr+2);
        // fprintf(stderr, "read: %d %d %d\n", arr[0], arr[1], arr[2]);
    }
    int* data = (int*) scatter(mpi, 0, (void *) arr, INT, 3);
    if (me == 0) {
        free(arr); 
    }


    data[0] *= 2;
    fprintf(stderr, "me: %d, data: %d\n", me, data[0]);
    int* res = (int*) gather(mpi, 1, (void *) data, INT, 3);
    free(data);

    if (me == 1) {
        fprintf(stderr, "res: %d %d %d\n", res[0], res[1], res[2]);
        free(res);
    }

    kill(mpi);
    fprintf(stderr, "End!\n");
    return 0;
}
