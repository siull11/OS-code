#include "mpi.h"

int** createPipes(int n);
int createProcesses(int n, int* pids);
void connectPipes(mpi* mpi);

mpi* init(int n) {
    mpi* instance = (mpi*) malloc(sizeof(mpi));
    instance->n = n;
    
    // Allocate space for pid array
    instance->pids = (int*) malloc(sizeof(int) * n);
    if (instance->pids == NULL) {
        perror("Allocation failed");
        exit(EXIT_FAILURE);
    }
    
    instance->pipes = createPipes(n);
    instance->me = createProcesses(n, instance->pids);
    connectPipes(instance);

    return instance;

}

void kill(mpi* mpi) {
    // Close all pipes
    for (int i = 0; i < mpi->n; i++) { //ksk nt denna typ av close???
        if (mpi->me == i) close(mpi->pipes[i][READ_END]);
        else close(mpi->pipes[i][WRITE_END]);
    }

    // Free memory // flytta upp ovan exit???
    free(mpi->pids);
    for (int i = 0; i < mpi->n; i++) {
        free(mpi->pipes[i]);
    }
    free(mpi->pipes);
    free(mpi);

    // If not the parent process, exit
    if (mpi->me != 0) {
        exit(EXIT_SUCCESS);
    }
}

int** createPipes(int n) {
    // Allocate space for filedescriptor array
    int** fd = (int**) malloc(sizeof(int*) * n);
    if (fd == NULL) {
        perror("Allocation failed");
        exit(EXIT_FAILURE);
    }


    // Create and allocate space for n pipes
    for (int i = 0; i < n; i++) {
        fd[i] = (int*) malloc(sizeof(int) * 2);
        if (fd[i] == NULL) {
            perror("Allocation failed");
            exit(EXIT_FAILURE);
        }
        if (pipe(fd[i]) < 0) {
            perror("Pipe creation failed");
            exit(EXIT_FAILURE);
        }
    }

    return fd;
}

int createProcesses(int n, int *pids) {
    pids[0] = getpid();
    // Create n-1 new processes
    int i;
    for (i = 1; i < n; i++) {

        // Create a new process
        int pid = fork();
        if (pid < 0) {
            perror("Fork failed");
            exit(EXIT_FAILURE);
        }

        if (pid == 0) { // Child process
            break;
        } else { // Parent process
            pids[i] = pid;
        }
    }
    return i % n;
}

void connectPipes(mpi* mpi) {
    for (int i = 0; i < mpi->n; i++) { //ksk nt denna typ av close???
        if (mpi->me == i) close(mpi->pipes[i][WRITE_END]);
        else close(mpi->pipes[i][READ_END]);
    }
}
