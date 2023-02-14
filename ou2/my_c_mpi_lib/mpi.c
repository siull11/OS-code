// ASSUMES n % np == 0
/* FURTHER IMPROVMENTS:
    All send / recev with id
    buffrad läsning (läs från buf först, sen läs tills rätt meddelande hittats och fyll buf) / alt. 2 trådar, en läser till cirkulär buf andra processar datan från buf
    chunkad skrivning och läsning + read / write multiple times before crash / fcntl för att öka pipe buf size (https://stackoverflow.com/questions/5218741/set-pipe-buffer-size)
*/

// Libs
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include "mpi.h"

// Constants
#define READ_END 0
#define WRITE_END 1

// Macro
#define Err(s) {perror(s); exit(EXIT_FAILURE);}

// Struct
struct mpi {
    int **pipes;
    char n;
    char rank;
};

int** createPipes(char n);
char createProcesses(char n);

char mpi_getRank(mpi* mpi) {
    return mpi->rank;
}

mpi* mpi_init(char n) {
    mpi* instance = (mpi*) malloc(sizeof(mpi));
    instance->n = n;
    
    instance->pipes = createPipes(n);
    instance->rank = createProcesses(n);

    // Close unused pipes for each process
    for (int i = 0; i < instance->n; i++) {
        if (instance->rank == i) close(instance->pipes[i][WRITE_END]);
        else close(instance->pipes[i][READ_END]);
    }

    return instance;
}

void mpi_kill(mpi* mpi, bool end) {
    char me = mpi->rank;

    // Close all pipes
    for (int i = 0; i < mpi->n; i++) {
        if (me == i) close(mpi->pipes[i][READ_END]);
        else close(mpi->pipes[i][WRITE_END]);
    }

    // Free memory
    for (int i = 0; i < mpi->n; i++) free(mpi->pipes[i]);
    free(mpi->pipes);
    free(mpi);

    // If end and not the parent process, exit
    if (end && me != 0) exit(EXIT_SUCCESS);
}

int** createPipes(char n) {
    // Allocate space for filedescriptor array
    int** fd = (int**) malloc(sizeof(int*) * n);
    if (fd == NULL) Err("Allocation failed");

    // Create and allocate space for n pipes
    for (int i = 0; i < n; i++) {
        fd[i] = (int*) malloc(sizeof(int) * 2);
        if (fd[i] == NULL) Err("Allocation failed");
        if (pipe(fd[i]) < 0) Err("Pipe creation failed");
    }

    return fd;
}

char createProcesses(char n) {
    // Create n-1 new processes
    char i;
    for (i = 1; i < n; i++) {
        // Create a new process
        int pid = fork();
        if (pid < 0) Err("Fork failed");

        if (pid == 0) break; // Child process
    }

    return i % n;
}

void mpi_send(mpi* mpi, char to, void* val, int size) {
    // Write to pipe
    if (write(mpi->pipes[(int)to][WRITE_END], val, size) < 0) Err("Write failed");
}

void mpi_receive(mpi* mpi, void* res, int size) {
    // Read from pipe
    if (read(mpi->pipes[(int)mpi->rank][READ_END], res, size) < 0) Err("Read failed");
}

void mpi_scatter(mpi* mpi, char from, void* val, void* res, int type, int len) {
    int block = len/mpi->n;
    int resSize = type*block;
    if (mpi->rank == from) { // I am sender
        // Send to all processes
        for (char i = 0; i < mpi->n; i++) if (i != mpi->rank) mpi_send(mpi, i, ((char*) val)+i*resSize, type*block);
        // Copy to myself
        memcpy(res, ((char*) val)+mpi->rank*resSize, resSize);
    } else mpi_receive(mpi, res, type*block); // I am receiver
}

void mpi_gather(mpi* mpi, char to, void* val, void* res, int type, int len) {
    int block = len/mpi->n;
    int resSize = type*block;
    if (mpi->rank == to) { // I am receiver
        memcpy(((char*) res)+mpi->rank*resSize, val, resSize); // Copy to myself
        // Receive from all processes
        void* msg = malloc(type*block+1);
        for (char i = 0; i < mpi->n; i++) if (i != mpi->rank) {
            mpi_receive(mpi, msg, type*block+1);
            char* bytes = (char*) msg;
            memcpy(((char*) res)+(*bytes)*resSize, bytes+1, resSize);
        }
        free(msg);
    } else { // I am sender
        void* msg = malloc(resSize+1);
        *((char*) msg) = mpi->rank;
        memcpy(((char*) msg)+1, val, resSize);
        mpi_send(mpi, to, msg, type*block+1);
        free(msg);
    }
}
