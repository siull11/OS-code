// mpiCC test1.cpp my_mpi.cpp -o test1
// mpirun -np 2 ./test1 1000000 0

// CALC SUM

#include <iostream>
#include <vector>
#include <cmath>

#include <mpi.h>
#include "my_mpi.h"

#define ROOT 0

using namespace std;

int main(int argc, char* argv[]) {
    // Initialize mpi
    int np, me;
    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &np);
    MPI_Comm_rank(MPI_COMM_WORLD, &me);

    // Check command line parameters
    if (argc < 2 || argc > 3) {
        cout << "Usage: " << argv[0] << " num_elem [rand_seed]" << endl;
        exit(EXIT_FAILURE);
    }
    // Read command line parameters
    unsigned int n = stoi(argv[1]);
    if (argc == 3) srand(stoi(argv[2]));
    else srand(time(0));

    unsigned int blockSize = n/np;

    // Generate random vectors
    vector<double> vec(me == ROOT ? n : blockSize);
    if (me == ROOT) for (unsigned long i = 0; i < n; i++) vec[i] = pow(-1, i) * ((double) rand())/100000;;
    // if (me == ROOT) for (unsigned long i = 0; i < n; i++) vec[i] = 1;

    double t0;
    if (me == ROOT) t0 = MPI_Wtime();

    // Scatter vector
    My_MPI_Scatter(&vec[0], blockSize, MPI_DOUBLE, &vec[0], blockSize, MPI_DOUBLE, ROOT, MPI_COMM_WORLD);
    if (me == ROOT) vec.resize(blockSize);

    // Do work
    double sum = 0;
    for (unsigned long i = 0; i < vec.size(); i++) sum += vec[i];

    // Gather results
    if (me == ROOT) vec.resize(np);
    My_MPI_Gather((void*) &sum, 1, MPI_DOUBLE, &vec[0], 1, MPI_DOUBLE, ROOT, MPI_COMM_WORLD);

    // Calc, print results
    if (me == ROOT) {
        double total = 0;
        for (unsigned long i = 0; i < vec.size(); i++) total += vec[i];
        double t1 = MPI_Wtime();
        cout << np << "," << total << "," << t1-t0 << endl;
    }

    // Finalize mpi
    MPI_Finalize();
    return 0;
}