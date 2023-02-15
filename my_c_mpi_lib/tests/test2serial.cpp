// Serially calculate the amount of primes under n

// g++ -Wall test2serial.cpp -o test2serial && ./test2serial 10
// time ./test2serial 10000

#include <iostream>
#include <vector>
#include <cmath>

using namespace std;

int main(int argc, char* argv[]) {
    // Check command line parameters
    if (argc != 2) {
        cout << "Usage: " << argv[0] << " max_num" << endl;
        exit(EXIT_FAILURE);
    }
    // Read command line parameters
    unsigned int n = stoi(argv[1]);

    // Check amount of primes under n
    int num = 0;
    for (unsigned long i = 2; i < n; i++) {
        for (unsigned long j = 2; j <= i; j++) {
            if (j == i) num++;
            if (i % j == 0) break;
        }
    }

    // Print results
    cout << "Num: " << num << endl;

    return 0;
}