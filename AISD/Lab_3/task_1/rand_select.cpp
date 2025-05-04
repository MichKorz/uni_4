#include <iostream>
#include <random>

std::random_device rd;                // Non-deterministic seed
std::mt19937 gen(rd());               // Mersenne Twister engine

long long comps = 0;
long long swaps = 0;

void insertionSort(int *arr, int n)
{
    for (int i = 1; i < n; ++i) 
    {
        int key = arr[i];
        int j = i - 1;

        ++comps;
        while (j >= 0 && arr[j] > key) 
        {
            ++comps;
            ++swaps;
            arr[j + 1] = arr[j];
            j = j - 1;
        }
        ++swaps;
        arr[j + 1] = key;
    }
}

long long partiton(int *A, long long p, long long r)
{
    int x = A[r];
    long long i = p - 1;
    for (int j = p; j <= r - 1; j++)    // TODO Make sure that j <=
    {
        comps++;
        if (A[j] <= x)
        {
            swaps++;
            i++;
            int t = A[i];   // TODO Add a swap function
            A[i] = A[j];
            A[j] = t;
        }
    }
    swaps++;
    int t = A[i + 1];   // TODO Add a swap function
    A[i + 1] = A[r];
    A[r] = t;

    return i + 1;
}

long long randomized_partiton(int *A, long long p, long long r)
{
    std::uniform_int_distribution<long long> dist(p, r); // Inclusive range [p, r]
    long long i = dist(gen);
    swaps++;
    int t = A[i];   // TODO Add a swap function
    A[i] = A[r];
    A[r] = t;

    return partiton(A, p, r);
}

int randomized_select(int *A, long long p, long long r, long long i)
{
    if (p == r) return A[p];

    long long q = randomized_partiton(A, p, r);
    long long k = q - p + 1;

    if (i == k) return A[q];
    else if (i < k) return randomized_select(A, p, q - 1, i);
    else return randomized_select(A, q + 1, r, i - k);
}

int main()
{
    long long n;
    std::cin >> n;

    long long i;
    std::cin >> i;

    int m;  //Burner parameter for later flexibility
    std::cin >> m;

    int A[n];

    for (long long j = 0; j < n; j++) 
    { 
        std::cin >> A[j];
        std::cout << A[j] << " ";
    }
    std::cout << std::endl;

    int result = randomized_select(A, 0, n - 1, i); 

    for (long long j = 0; j < n; j++) 
    { 
        std::cout << A[j] << " ";
    }
    std::cout << std::endl;

    std::cout << "statystyka: " << result << std::endl;

    insertionSort(A, n);

    for (long long j = 0; j < n; j++) 
    { 
        std::cout << A[j] << " ";
    }
    std::cout << std::endl;
}
