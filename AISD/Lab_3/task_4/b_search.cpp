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
    int n;
    std::cin >> n;

    int i;
    std::cin >> i;

    int m;
    std::cin >> m;

    int A[n];

    for (int j = 0; j < n; j++)
    {
        std::cin >> A[j];
    }

    bool exists = binary_search(A, 0, n - 1, i);

    std::cout << comps << std::endl;
    std::cout << comps << std::endl;
}