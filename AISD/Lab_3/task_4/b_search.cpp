#include <iostream>

long long comps = 0;
long long swaps = 0;

bool binary_search(int *A, long long p, long long r, int x) 
{
    if (p >= r) return false; 

    long long mid = p + (r - p) / 2;

    comps++;

    if (A[mid] == x) return true; 
    else if (A[mid] < x) return binary_search(A, mid + 1, r, x);
    else return binary_search(A, p, mid -1, x);
}

int main()
{
    int test_data[10] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};

    bool exists = binary_search(test_data, 0, 9, 4);

    std::cout<< exists << std::endl;
}