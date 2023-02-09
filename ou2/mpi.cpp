#include "mpi.h"
#include <unistd.h>

// Constants
#define READ_END 0
#define WRITE_END 1

// Macro
#define Err(s) {perror(s); exit(EXIT_FAILURE);}

int** createPipes(int n);
int createProcesses(int n);

static int me;

int getMe() {
    return me;
}

mpi* init(int n) {
    mpi* instance = (mpi*) malloc(sizeof(mpi));
    instance->n = n;
    
    instance->pipes = createPipes(n);
    me = createProcesses(n);

    // Close unused pipes for each process
    for (int i = 0; i < instance->n; i++) {
        if (me == i) close(instance->pipes[i][WRITE_END]); // Göra så kan skicka till sig själv???
        else close(instance->pipes[i][READ_END]);
    }

    return instance;
}

void kill(mpi* mpi) {
    // Close all pipes
    for (int i = 0; i < mpi->n; i++) {
        if (me == i) close(mpi->pipes[i][READ_END]);
        else close(mpi->pipes[i][WRITE_END]);
    }

    // Free memory
    for (int i = 0; i < mpi->n; i++) free(mpi->pipes[i]);
    free(mpi->pipes);
    free(mpi);

    // If not the parent process, exit
    if (me != 0) exit(EXIT_SUCCESS);
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

void send(mpi* mpi, int to, void* val, int size, int len) {
    // Write to pipe
    if (write(mpi->pipes[to][WRITE_END], val, size*len) < 0) Err("Write failed");
}

void* receive(mpi* mpi, int size, int len) { // fixa struct to send????
    void* val = malloc(size*len);
    // Read from pipe
    if (read(mpi->pipes[me][READ_END], val, size*len) < 0) Err("Read failed");
    return val;
}

void* scatter(mpi* mpi, int from, void* val, int size, int len) {  // fixa struct to send????
    int block = len/mpi->n;
    void* msg = malloc(size*block);
    if (me == from) { // I am sender
        // Send to all processes
        for (int i = 0; i < mpi->n; i++) if (i != me) send(mpi, i, val, size, len);
        for (int j = 0; j < block; j++) ((char*) msg)[j] = ((char*) val)[j+me*block]; // FUNKAR BARA FÖR CHAR!!!
    } else msg = receive(mpi, size, block); // I am receiver
    return msg;
}