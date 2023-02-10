// g++ -Wall test1parallel.cpp mpi.c -o test1parallel && ./test1parallel 2 10 0

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

    // Do work
    double sum = 0;
    for (unsigned long i = 0; i < vec.size(); i++) {
        sum += vec[i];
    }

    // Gather results
    if (me == 0) vec.resize(np);
    mpi_gather(mpi, 0, (void *) &sum, &vec[0], DOUBLE, np);

    // Calc, print results
    if (me == 0) {
        double total = 0;
        for (unsigned long i = 0; i < vec.size(); i++) {
            // cout << vec[i] << endl;
            total += vec[i];
        }
        cout << "Total: " << total << endl;
    }

    mpi_kill(mpi);
    return 0;
}