#pragma once

// Lib
#include <stdbool.h>

// Constants
#define INT sizeof(int)
#define CHAR sizeof(char)
#define FLOAT sizeof(float)
#define DOUBLE sizeof(double)

// Typedef
typedef struct mpi mpi;

// Functions
char mpi_getRank(mpi* mpi);
mpi* mpi_init(char n);
void mpi_kill(mpi* mpi, bool end);
void mpi_send(mpi* mpi, char to, void* val, int size);
void mpi_receive(mpi* mpi, void* res, int size);
void mpi_scatter(mpi* mpi, char from, void* val, void* res, int type, int len);
void mpi_gather(mpi* mpi, char to, void* val, void* res, int type, int len);
