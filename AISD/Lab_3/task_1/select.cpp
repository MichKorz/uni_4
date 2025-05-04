#include <iostream>
#include <vector>

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

void swap_elements(int *a, int *b) 
{
    ++swaps;
    int temp = *a;
    *a = *b;
    *b = temp;
}

long long partition(int *A, long long p, long long r) {
    int x = A[r];
    long long i = p - 1;
    for (long long j = p; j <= r - 1; j++) {
        ++comps;
        if (A[j] <= x) {
            i++;
            swap_elements(&A[i], &A[j]);
        }
    }
    swap_elements(&A[i + 1], &A[r]);
    return i + 1;
}


long long specific_partition(int *A, long long p, long long r, int x) 
{
    long long index = p;
    while (index <= r) 
    { 
         ++comps; 
         if (A[index] == x) break; 
         ++index;
    }
    swap_elements(&A[index], &A[r]); 
    
    return partition(A, p, r); 
}


int select(int *A, int batchSize, long long p, long long r, long long i) {
    if (p == r) 
    {
        return A[p];
    }

    long long n_prime = r - p + 1; 
    long long mediansNum = (n_prime + batchSize - 1) / batchSize; 
    
    std::vector<int> medians(mediansNum); 
    
    for (long long group_idx = 0; group_idx < mediansNum; ++group_idx) {
        long long group_start = p + group_idx * batchSize;
        long long group_end = std::min(group_start + batchSize - 1, r); 
        int current_group_size = group_end - group_start + 1;

        std::vector<int> group(current_group_size);
        for(int k=0; k < current_group_size; ++k) {
            group[k] = A[group_start + k];
        }

        insertionSort(group.data(), current_group_size); 

        medians[group_idx] = group[current_group_size / 2]; 
    }

    int x; 
    if (mediansNum == 1) 
    {
        x = medians[0];
    } 
    else 
    {
        long long medianOfMediansRank = (mediansNum / 2) + 1; 
        x = select(medians.data(), batchSize, 0, mediansNum - 1, medianOfMediansRank);
    }

    long long q = specific_partition(A, p, r, x); 
    long long k = q - p + 1; 

    if (i == k) 
    { 
        return x; 
    } 
    else if (i < k) return select(A, batchSize, p, q - 1, i);
    else return select(A, batchSize, q + 1, r, i - k); 
}


int main() 
{
    long long n;
    std::cin >> n;

    long long i; 
    std::cin >> i;

    int m; 
    std::cin >> m;

    std::vector<int> A(n); 

    for (long long j = 0; j < n; j++) 
    { 
        std::cin >> A[j];
        std::cout << A[j] << " ";
    }
    std::cout << std::endl;

    int result = select(A.data(), m, 0, n - 1, i); 

    for (long long j = 0; j < n; j++) 
    { 
        std::cout << A[j] << " ";
    }
    std::cout << std::endl;

    std::cout << "statystyka: " << result << std::endl;

    insertionSort(A.data(), n);

    for (long long j = 0; j < n; j++) 
    { 
        std::cout << A[j] << " ";
    }
    std::cout << std::endl;
    
    return 0;
}