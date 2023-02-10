#pragma once

#define INT sizeof(int)
#define CHAR sizeof(char)
#define FLOAT sizeof(float)
#define DOUBLE size_t(double)

// Structs
typedef struct mpi {
    int **pipes;
    int n;
} mpi;

// Functions
char mpi_getRank();
mpi* mpi_init(int n);
void mpi_kill(mpi* mpi);
void mpi_send(mpi* mpi, char to, void* val, int size);
void* mpi_receive(mpi* mpi, int size, void* res);
void* mpi_scatter(mpi* mpi, char from, void* val, int type, int len);
void* mpi_gather(mpi* mpi, char to, void* val, int type, int len);
