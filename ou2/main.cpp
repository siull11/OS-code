// g++ -Wall main.cpp mpi.cpp -o exec

#include "mpi.h"

int main(int argc, char* argv[]) {
    mpi* mpi = init(6);
    printf("Hello world!\n");
    kill(mpi);
    return 0;
}