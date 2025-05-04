//
// Created by Sara on 25.04.2025.
//
#include <iostream>
#include <random>
using namespace std;

int main(int argc, const char * argv[]) {
    if (argc != 2) {
        cerr << "Needed one argument. Got: " << argc-1 << endl;
        return -1;
    }

    int n = atoi(argv[1]);
    if (n <= 0) {
        cerr << "Needed number must be greater than 0" << endl;
    }

    random_device rd;
    mt19937 rng(rd());
    uniform_int_distribution<int> bin(1, n);
    uniform_int_distribution<int> bin2(0, 2*n-1);
    
    int k = bin(rng);
    cout << n << " " << k << " ";
    for (int i = 0; i < n; i++) {
        cout << bin2(rng) << " ";
    }

    return 0;
}
