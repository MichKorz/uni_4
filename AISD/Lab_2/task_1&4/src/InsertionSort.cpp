#include <iostream>
#include <vector>

using namespace std;

/* Function to sort array using insertion sort */
void insertionSort(vector<int>& arr, int n, int& comparisons, int& swaps)
{
    for (int i = 1; i < n; ++i) 
    {
        int key = arr[i];
        int j = i - 1;

        // Compare with previous elements
        while (j >= 0 && arr[j] > key) 
        {
            comparisons++; // Count comparison
            arr[j + 1] = arr[j]; // Shift element
            swaps++; // Count swap (shift)
            j = j - 1;
        }
        comparisons++; // Final comparison when while condition fails

        arr[j + 1] = key;
        if (j + 1 != i) 
        {
            swaps++; // Count the final placement of key if it moved
        }
    }
}

/* A utility function to print array of size n */
void printArray(vector<int>& arr, int n)
{
    for (int i = 0; i < n; ++i)
        cout << arr[i] << " ";
    cout << endl;
}

// Driver method
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

    int comparisons = 0, swaps = 0;
    insertionSort(arr, n, comparisons, swaps);
    printArray(arr, n);
    cout << endl;

    cout << "Comparisons: " <<comparisons << endl;
    cout << "Swaps: " << swaps << endl;

    return 0;
}
