#include <iostream>
#include <vector>
using namespace std;

int comparisons = 0, swaps = 0;

void insertionSort(vector<int>& arr, int n)
{
    for (int i = 1; i < n; ++i) 
    {
        int key = arr[i];
        int j = i - 1;

        ++comparisons;
        while (j >= 0 && arr[j] > key) 
        {
            ++comparisons;
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

long long partition(vector<int>& A, long long p, long long r) {
    int x = A[r];
    long long i = p - 1;
    for (long long j = p; j <= r - 1; j++) {
        ++comparisons;
        if (A[j] <= x) {
            i++;
            swap_elements(&A[i], &A[j]);
        }
    }
    swap_elements(&A[i + 1], &A[r]);
    return i + 1;
}


long long specific_partition(vector<int>& A, long long p, long long r, int x) 
{
    long long index = p;
    while (index <= r) 
    { 
         ++comparisons; 
         if (A[index] == x) break; 
         ++index;
    }
    swap_elements(&A[index], &A[r]); 
    
    return partition(A, p, r); 
}

int select(vector<int>& A, int batchSize, long long p, long long r, long long i) {
    // Base case: If the subarray has only one element
    if (p == r) { // Simplified base case: p and r point to the same element
        return A[p];
    }

    // 1. Divide A[p..r] into groups of batchSize
    long long n_prime = r - p + 1; // Number of elements in current subarray
    long long mediansNum = (n_prime + batchSize - 1) / batchSize; // Ceiling division
    
    // Use std::vector instead of VLA for portability and safety
    std::vector<int> medians(mediansNum); 
    
    for (long long group_idx = 0; group_idx < mediansNum; ++group_idx) {
        long long group_start = p + group_idx * batchSize;
        long long group_end = std::min(group_start + batchSize - 1, r); // Ensure not exceeding r
        int current_group_size = group_end - group_start + 1;

        // Use a temporary array or vector for the group
        std::vector<int> group(current_group_size);
        for(int k=0; k < current_group_size; ++k) {
            group[k] = A[group_start + k];
        }

        // Sort the small group using insertion sort
        insertionSort(group, current_group_size); // Pass pointer to vector data

        // Find the median of the group and store it
        medians[group_idx] = group[current_group_size / 2]; // Median index (0-based)
    }

    // 2. Find the median of the medians ('x') recursively
    int x; // Pivot element (median of medians)
    if (mediansNum == 1) {
        x = medians[0];
    } else {
        // Recursively find the median of the 'medians' array.
        // The rank we want is the middle element (1-based index).
        long long medianOfMediansRank = (mediansNum / 2) + 1; // e.g., for 5 medians, rank is 3.
        x = select(medians, batchSize, 0, mediansNum - 1, medianOfMediansRank);
    }


    // 3. Partition the original array A[p..r] around the pivot x
    // Note: specific_partition finds x, swaps it to A[r], then calls partition.
    long long q = specific_partition(A, p, r, x); 
                                      // q is the final 0-based index of pivot x

    // 4. Calculate the rank k of the pivot x within A[p..r] (1-based)
    long long k = q - p + 1; 

    // 5. Check pivot's rank and recurse
    if (i == k) { // The pivot is the i-th element
        return x; // Or return A[q] which is the same
    } else if (i < k) { // The i-th element is in the left partition A[p..q-1]
        // Check if left partition exists (q > p)
        if (q > p) {
             return select(A, batchSize, p, q - 1, i); // Rank i remains the same
        } else {
             // This case should ideally not happen if partitioning is correct 
             // and i < k, but handle defensively. Maybe return A[p]? Or error?
             // If q == p, it means the pivot was the smallest element. If i < k (i < 1),
             // this is an invalid input 'i'. Let's assume valid 'i' >= 1.
             // If k=1 (pivot is smallest) and i < k, this is impossible.
             // If q > p is false, then q=p. k = p-p+1 = 1. If i<k then i<1, impossible.
             // So q > p check might be redundant assuming i>=1.
             return select(A, batchSize, p, q - 1, i); 
        }
       
    } else { // The i-th element is in the right partition A[q+1..r]
         // Check if right partition exists (q < r)
         if (q < r) {
              return select(A, batchSize, q + 1, r, i - k); // Adjust rank to i-k
         } else {
              // Similar to the left side, this case (q=r, pivot is largest, but i > k)
              // implies an issue or impossible scenario if i > n_prime. Assume valid i.
              // If k = n_prime (pivot is largest) and i > k, this is impossible.
              // If q < r is false, then q=r. k=r-p+1=n_prime. If i>k, then i>n_prime, impossible.
              return select(A, batchSize, q + 1, r, i - k);
         }
    }
}

void quickSort(vector<int>& arr, int low, int high) 
{
    int pivot = select(arr, 5, low, high, (low + (high - low)/2));

    if (low < high) 
    {
        // Partition and get pivot index
        int pi = specific_partition(arr, low, high, pivot);

        // Recursive calls
        quickSort(arr, low, pi - 1);
        quickSort(arr, pi + 1, high);
    }
}

int main() 
{
    int n;
    cin >> n;
    vector<int> arr(n);
    
    for (int i = 0; i < n; i++)
    {
        cin >> arr[i];
        cout << arr[i];
    }
    cout << endl;

    quickSort(arr, 0, n - 1);
  
    for (int i = 0; i < n; i++) 
    {
        cout << arr[i] << " ";
    }
    cout << endl;
    
    cout << "Comparisons: " <<comparisons << endl;
    cout << "Swaps: " << swaps << endl;
    return 0;
}
