#include <iostream>
#include <vector>

using namespace std;

// Global counters
int comparisons = 0;
int swaps = 0;

// Merge function to combine two sorted halves
void merge(vector<int>& arr, int left, int mid, int right) {
    int n1 = mid - left + 1;
    int n2 = right - mid;

    // Temporary arrays
    vector<int> L(n1), R(n2);

    // Copy data to temporary arrays
    for (int i = 0; i < n1; i++) {
        L[i] = arr[left + i];
        swaps++; // Counting copies as swaps
    }
    for (int j = 0; j < n2; j++) {
        R[j] = arr[mid + 1 + j];
        swaps++; // Counting copies as swaps
    }

    int i = 0, j = 0, k = left;

    // Merge the two halves back into arr
    while (i < n1 && j < n2) {
        comparisons++; // Counting comparisons
        if (L[i] <= R[j]) {
            arr[k] = L[i];
            i++;
        } else {
            arr[k] = R[j];
            j++;
        }
        swaps++; // Counting element movement
        k++;
    }

    // Copy any remaining elements from L[]
    while (i < n1) {
        arr[k] = L[i];
        i++;
        k++;
        swaps++; // Counting element movement
    }

    // Copy any remaining elements from R[]
    while (j < n2) {
        arr[k] = R[j];
        j++;
        k++;
        swaps++; // Counting element movement
    }
}

// Merge Sort function
void mergeSort(vector<int>& arr, int left, int right) {
    if (left < right) {
        int mid = left + (right - left) / 2;

        // Sort first and second halves
        mergeSort(arr, left, mid);
        mergeSort(arr, mid + 1, right);

        // Merge the sorted halves
        merge(arr, left, mid, right);
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

    mergeSort(arr, 0, n - 1);

    // Output sorted array
    /*for (int num : arr) {
        cout << num << " ";
    }
    cout << endl;*/

    // Output stats
    cout << comparisons << endl;
    cout << swaps << endl;

    return 0;
}
