#pragma once

//Flytta till mpi.cpp???
// Libs
#include <stdlib.h>
#include <stdio.h>

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
int getMe();
mpi* init(int n);
void kill(mpi* mpi);
void send(mpi* mpi, int to, void* val, int type, int len);
void* receive(mpi* mpi, int type, int len);
