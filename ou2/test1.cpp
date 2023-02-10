// g++ -Wall test1.cpp mpi.c -o test1

// #include <stdlib.h>
#include <iostream>
#include <vector>
#include <cmath>

#include "mpi.h"

using namespace std;

int main(int argc, char* argv[]) {
    // Check command line parameters
    if (argc < 3 || argc > 4) {
        cout << "Usage: " << argv[0] << " num_proc num_elem [rand_seed]" << endl;
        exit(EXIT_FAILURE);
    }
    // Read command line parameters
    unsigned int np = stoi(argv[1]);
    unsigned int n = stoi(argv[2]);
    if (argc == 4) srand(stoi(argv[3]));
    else srand(time(0));

    unsigned int blockSize = n/np;

    // Initialize mpi
    mpi* mpi = mpi_init(np);
    char me = mpi_getRank(mpi);

    // Generate random vectors
    vector<double> vec(me == 0 ? n : blockSize, 0);
    if (me == 0) {
        vec.resize(n);
        for (unsigned long i = 0; i < vec.size(); i++) {
            double val = pow(-1, i) * ((double) rand())/100000;
            vec[i] = val;
        }
    }

    // Scatter vector
    mpi_scatter(mpi, 0, (void *) &vec[0], &vec[0], DOUBLE, n);

    if (me == 0) vec.resize(blockSize);
    
    if (me == 0) {
        for (unsigned long i = 0; i < vec.size(); i++) {
            cout << vec[i] << "," ;
        }
        cout << endl;
    }

    mpi_kill(mpi);
    return 0;
}