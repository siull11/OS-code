#include "mpi.h"

mpi* init(int n) {
    //int** pipes = createPipes(n);
    printf("Hello world!\n");

}

void kill(mpi* mpi) {
    // Close all pipes !!!

    // Kill all processes
    for (int i = 1; i < mpi->n; i++) { // ksk nt första + fel ordning???
        kill(mpi->pids[i], SIGKILL);
    }

    // Free memory
    free(mpi->pids);
    for (int i = 0; i < mpi->n; i++) {
        free(mpi->pipes[i]);
    }
    free(mpi->pipes);
    free(mpi);
}

/* createPipes function:
Allocates memory for and creates n pipes.

Argument:
The amount of pipes to create, n.

Returns:
The filedescriptors to both ends of all pipes.
*/
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

int* createProcesses(int n) {
    // Create n-1 new processes
    for (int i = 1; i < n; i++) {

        // Create a new process
        int pid = fork();
        if (pid < 0) {
            perror("Fork failed");
            exit(EXIT_FAILURE);
        }

        if (pid == 0) { // Child process
            
            // Close all pipes except the one to the parent
            for (int j = 0; j < n; j++) {
                if (j != i) {
                    close(pipes[j][READ_END]);
                    close(pipes[j][WRITE_END]);
                }
            }
            // Close the read end of the pipe to the parent
            close(pipes[i][READ_END]);
            // Run the process
            process(i);
        }
    }
}
