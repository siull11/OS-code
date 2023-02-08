// gcc -Wall main.cpp mpi.cpp -o exec

#include "mpi.h"

int main(int argc, char* argv[]) {
    mpi* mpi = init(2);
    fprintf(stderr, "Hello world!\n");
    kill(mpi);
    fprintf(stderr, "End!\n");
    return 0;
}