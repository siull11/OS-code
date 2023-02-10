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

int** createPipes(int n);
int createProcesses(int n);

static char myRank;

char mpi_getRank() {
    return myRank;
}

mpi* mpi_init(int n) {
    mpi* instance = (mpi*) malloc(sizeof(mpi));
    instance->n = n;
    
    instance->pipes = createPipes(n);
    myRank = createProcesses(n);

    // Close unused pipes for each process
    for (int i = 0; i < instance->n; i++) {
        if (myRank == i) close(instance->pipes[i][WRITE_END]); // Göra så kan skicka till sig själv???
        else close(instance->pipes[i][READ_END]);
    }

    return instance;
}

void mpi_kill(mpi* mpi) {
    // Close all pipes
    for (int i = 0; i < mpi->n; i++) {
        if (myRank == i) close(mpi->pipes[i][READ_END]);
        else close(mpi->pipes[i][WRITE_END]);
    }

    // Free memory
    for (int i = 0; i < mpi->n; i++) free(mpi->pipes[i]);
    free(mpi->pipes);
    free(mpi);

    // If not the parent process, exit
    if (myRank != 0) exit(EXIT_SUCCESS);
}

int** createPipes(int n) {
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

int createProcesses(int n) {
    // Create n-1 new processes
    int i;
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

void* mpi_receive(mpi* mpi, int size, void* res) {
    if (res == NULL) res = malloc(size); // Allocate memory if not already allocated
    // Read from pipe
    if (read(mpi->pipes[(int)myRank][READ_END], res, size) < 0) Err("Read failed");
    return res;
}

void* mpi_scatter(mpi* mpi, char from, void* val, int type, int len) {
    int block = len/mpi->n;
    int resSize = type*block;
    void* res;
    if (myRank == from) { // I am sender
        // Send to all processes
        for (int i = 0; i < mpi->n; i++) if (i != myRank) mpi_send(mpi, i, ((char*) val)+i*resSize, type*block);
        // Copy to myself
        res = malloc(resSize);
        memcpy(res, ((char*) val)+myRank*resSize, resSize);
    } else res = mpi_receive(mpi, type*block, NULL); // I am receiver
    return res;
}

void* mpi_gather(mpi* mpi, char to, void* val, int type, int len) {
    int block = len/mpi->n;
    int resSize = type*block;
    void* res = NULL;
    if (myRank == to) { // I am receiver
        res = malloc(type*len);
        memcpy(((char*) res)+myRank*resSize, val, resSize); // Copy to myself
        // Receive from all processes
        for (int i = 0; i < mpi->n; i++) if (i != myRank) {
            void* msg = mpi_receive(mpi, type*block+1, NULL);
            char* bytes = (char*) msg;
            memcpy(((char*) res)+(*bytes)*resSize, bytes+1, resSize);
            free(msg);
        }
    } else { // I am sender
        void* msg = malloc(resSize+1);
        *((char*) msg) = myRank;
        memcpy(((char*) msg)+1, val, resSize);
        mpi_send(mpi, to, msg, type*block+1);
        free(msg);
    }
    return res;
}
