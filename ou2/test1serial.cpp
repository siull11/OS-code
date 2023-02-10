// g++ -Wall test1serial.cpp mpi.c -o test1serial && ./test1serial 10 0

#include <iostream>
#include <vector>
#include <cmath>

using namespace std;

int main(int argc, char* argv[]) {
    // Check command line parameters
    if (argc < 2 || argc > 3) {
        cout << "Usage: " << argv[0] << " num_elem [rand_seed]" << endl;
        exit(EXIT_FAILURE);
    }
    // Read command line parameters
    unsigned int n = stoi(argv[1]);
    if (argc == 3) srand(stoi(argv[2]));
    else srand(time(0));

    // Generate random vectors
    vector<double> vec(n, 0);
    vec.resize(n);
    for (unsigned long i = 0; i < vec.size(); i++) {
        double val = pow(-1, i) * ((double) rand())/100000;
        vec[i] = val;
    }

    // Calc, print results
    double sum = 0;
    for (unsigned long i = 0; i < vec.size(); i++) {
        sum += vec[i];
    }
    cout << "Total: " << sum << endl;

    return 0;
}