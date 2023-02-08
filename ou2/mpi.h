#pragma once

//Flytta till mpi.cpp???
// Libs
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

// Structs
typedef struct mpi {
    int **pipes;
    int n;
} mpi;

// Functions
mpi* init(int n);
void kill(mpi* mpi);
int getMe();
