#include <iostream>
#include <vector>
using namespace std;

int comparisons = 0, swaps = 0;

int partition(vector<int>& arr, int low, int high) 
{
    // Choose the pivot
    int pivot = arr[high];
    int i = low - 1;

    // Traverse arr[low..high] and move all smaller elements to the left
    for (int j = low; j <= high - 1; j++) 
    {
        comparisons++; // Count comparison
        if (arr[j] < pivot) {
            i++;
            swap(arr[i], arr[j]);
            swaps++; // Count swap
        }
    }
    
    // Move pivot after smaller elements and return its position
    swap(arr[i + 1], arr[high]);  
    swaps++; // Count pivot swap
    return i + 1;
}

// QuickSort function implementation
void quickSort(vector<int>& arr, int low, int high) 
{
    if (low < high) 
    {
        // Partition and get pivot index
        int pi = partition(arr, low, high);

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
    }

    quickSort(arr, 0, n - 1);
  
    /*for (int i = 0; i < n; i++) 
    {
        cout << arr[i] << " ";
    }*/
    
    cout << comparisons << endl;
    cout << swaps << endl;

    return 0;
}
