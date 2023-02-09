// gcc -Wall main.cpp mpi.cpp -o exec

#include "mpi.h"

int main(int argc, char* argv[]) {
    mpi* mpi = init(2);
    int me = getMe();
    if (me == 1) {
        send(mpi, 0, (void *) "Hello parallel world!", CHAR, 22);
    } else if (me == 0) {
        void* msg = receive(mpi, CHAR, 22);
        fprintf(stderr, "Received: %s\n", (char*) msg);
        free(msg);
    }

    kill(mpi);
    fprintf(stderr, "End!\n");
    return 0;
}
