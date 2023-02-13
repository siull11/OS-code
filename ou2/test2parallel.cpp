// Parallely calculate the amount of primes under n

// g++ -Wall test2parallel.cpp mpi.c -o test2parallel && ./test2parallel 2 10
// time ./test2parallel 2 10000

#include <iostream>
#include <vector>
#include <cmath>

#include "mpi.h"

using namespace std;

int main(int argc, char* argv[]) {
    // Check command line parameters
    if (argc != 3) {
        cout << "Usage: " << argv[0] << " num_proc max_num" << endl;
        exit(EXIT_FAILURE);
    }
    // Read command line parameters
    unsigned int np = stoi(argv[1]);
    unsigned int n = stoi(argv[2]);

    unsigned int blockSize = n/np;

    // Initialize mpi
    mpi* mpi = mpi_init(np);
    char me = mpi_getRank(mpi);

    vector<int> vec(me == 0 ? np : 1);
    // Generate bound
    if (me == 0) for (unsigned int i = 0; i < np; i++) vec[i] = (i+1)*blockSize-1;

    // Scatter vector
    mpi_scatter(mpi, 0, (void *) &vec[0], &vec[0], INT, np);

    // Check amount of primes within assigned span (work)
    int num = 0;
    int lower = me == 0 ? 2 : me*blockSize;
    int upper = vec[0];
    for (int i = lower; i < upper; i++) {
        for (int j = 2; j <= i; j++) {
            if (j == i) num++;
            if (i % j == 0) break;
        }
    }

    // Gather results
    mpi_gather(mpi, 0, (void *) &num, &vec[0], INT, np);

    // Calc, print results
    if (me == 0) {
        double total = 0;
        for (unsigned long i = 0; i < vec.size(); i++) {
            total += vec[i];
            cout << "v: " << vec[i] << endl;
        }
        cout << "Num: " << total << endl;
    }

    // Finalize mpi
    mpi_kill(mpi, false);
    return 0;
}