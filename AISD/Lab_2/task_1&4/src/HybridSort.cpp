#include <iostream>
#include <vector>

using namespace std;

int comparisons = 0, swaps = 0;

/* Function to sort a subarray using insertion sort */
void insertionSort(vector<int>& arr, int low, int high)
{
    for (int i = low + 1; i <= high; ++i) 
    {
        int key = arr[i];
        int j = i - 1;

        while (j >= low && arr[j] > key) 
        {
            comparisons++; // Count comparison
            arr[j + 1] = arr[j]; // Shift element
            swaps++; // Count shift
            j = j - 1;
        }
        comparisons++; // Last comparison that fails

        arr[j + 1] = key;
        swaps++; // Final placement of key
    }
}

/* Partition function for QuickSort */
int partition(vector<int>& arr, int low, int high) 
{
    int pivot = arr[high];
    int i = low - 1;

    for (int j = low; j <= high - 1; j++) 
    {
        comparisons++; // Count comparison
        if (arr[j] < pivot) 
        {
            i++;
            swap(arr[i], arr[j]);
            swaps++; // Count swap
        }
    }

    swap(arr[i + 1], arr[high]);  
    swaps++; // Count pivot swap
    return i + 1;
}

/* Hybrid QuickSort + Insertion Sort */
void quickSort(vector<int>& arr, int low, int high) 
{
    while (low < high) 
    {
        // If the size of the subarray is small, use Insertion Sort
        if (high - low + 1 <= 5) 
        {
            insertionSort(arr, low, high);
            return;
        }

        // Partition the array
        int pi = partition(arr, low, high);

        // Tail call optimization: always sort the smaller subarray first
        if (pi - low < high - pi) 
        {
            quickSort(arr, low, pi - 1);
            low = pi + 1; // Avoid extra recursion by updating low
        } else 
        {
            quickSort(arr, pi + 1, high);
            high = pi - 1;
        }
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
