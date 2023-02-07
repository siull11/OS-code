#pragma once


//Flytta till mpi.cpp???
// Libs
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <signal.h>

// Constants
#define READ_END 0
#define WRITE_END 1

// Structs
typedef struct mpi {
    int pid;
    int *pids;
    int **pipes;
    int n;
} mpi;

// Functions
mpi* init(int n);
void kill(mpi* mpi);
