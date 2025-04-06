#include <iostream>
#include <vector>

using namespace std;

// Global counters
int comparisons = 0;
int swaps = 0;

// Function to swap elements and count swaps
void swapElements(int &a, int &b) {
    swaps++;
    int temp = a;
    a = b;
    b = temp;
}

// Dual-Pivot Partition Function
pair<int, int> partition(vector<int>& arr, int low, int high) {
    comparisons++; // Comparing pivots

    // Ensure pivot1 is smaller than pivot2
    if (arr[low] > arr[high]) {
        swapElements(arr[low], arr[high]);
    }

    int pivot1 = arr[low], pivot2 = arr[high];

    int left = low + 1, right = high - 1, i = left;

    while (i <= right) {
        comparisons++; // Each loop involves a comparison

        if (arr[i] < pivot1) { // Move to left section
            swapElements(arr[i], arr[left]);
            left++;
        } else if (arr[i] > pivot2) { // Move to right section
            while (arr[right] > pivot2 && i < right) {
                right--;
                comparisons++;
            }
            swapElements(arr[i], arr[right]);
            right--;
            if (arr[i] < pivot1) {
                swapElements(arr[i], arr[left]);
                left++;
            }
        }
        i++;
    }

    // Place pivots in correct positions
    left--;
    right++;
    swapElements(arr[low], arr[left]);
    swapElements(arr[high], arr[right]);

    return {left, right}; // Return pivot positions
}

// Dual-Pivot QuickSort Function
void dualPivotQuickSort(vector<int>& arr, int low, int high) {
    if (low < high) {
        pair<int, int> pivots = partition(arr, low, high);

        dualPivotQuickSort(arr, low, pivots.first - 1);
        dualPivotQuickSort(arr, pivots.first + 1, pivots.second - 1);
        dualPivotQuickSort(arr, pivots.second + 1, high);
    }
}

// Main function to test sorting
int main() {
    int n;
    cin >> n;
    vector<int> arr(n);

    for (int i = 0; i < n; i++) {
        cin >> arr[i];
    }

    dualPivotQuickSort(arr, 0, n - 1);

    // Output sorted array
    /*for (int num : arr) {
        cout << num << " ";
    }
    cout << endl;*/

    // Output counts
    cout << comparisons << endl;
    cout << swaps << endl;

    return 0;
}
