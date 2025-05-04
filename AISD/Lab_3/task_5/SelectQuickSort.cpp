#include <iostream>
#include <fstream>
#include <random>
#include <chrono>
using namespace std;
using namespace std::chrono;

long long comparisons = 0;
bool showSteps = false;
int n;

bool isBigger(const int a, const int b) {
    comparisons++;
    return a > b;
}

void swapElements(int *array, const int i, const int j) {
    swap(array[i], array[j]);
}

void display(int *array)
{
    for (int i=0; i<n; i++) {
        int key = array[i];
        if(key < 10) cout << "0";
        cout << key << " ";
    }
    cout << endl;
}


void InsertSort(int *array, const int l, const int h, bool print = false) {
    for (int i = l + 1; i <= h; ++i) {
        int key = array[i];
        int j = i - 1;
        while (j >= l && isBigger(array[j], key)) {
            array[j + 1] = array[j];
            --j;
        }
        array[j + 1] = key;
    }
}

int Partition(int *array, int l, int r, int pivotValue) {
    // Find x in A[l..r] and move it to end
    int pivotIndex = l;
    while (pivotIndex <= r && array[pivotIndex] != pivotValue) pivotIndex++;
    swapElements(array, pivotIndex, r);
    int pivot = array[r];

    int i = l - 1;
    for (int j = l; j < r; ++j) {
        if (!isBigger(array[j], pivot)) {
            ++i;
            swapElements(array, i, j);
        }
    }
    swapElements(array, i + 1, r);
    return i + 1;
}

int Select(int *array, const int p, const int r, const int i) {
    if (p == r) return array[p];

    int length = r - p + 1;
    // if there are <= 5 than we can just sort it cuz it's like finding a median
    if (length <= 5) {
        InsertSort(array, p, r, false);
        return array[p + i - 1];
    }

    int numMedians = (length + 4) / 5;
    int *medians = new int[numMedians];

    for (int g = 0; g < numMedians; ++g) {
        int gl = p + g * 5;
        int gr = std::min(gl + 4, r);
        InsertSort(array, gl, gr, false);
        medians[g] = array[gl + (gr - gl) / 2];
    }

    int mom = Select(medians, 0, numMedians - 1, (numMedians + 1) / 2);
    delete[] medians;

    int q = Partition(array, p, r, mom);
    int k = q - p + 1;
    if (i == k) return array[q];
    else if (i < k) return Select(array, p, q - 1, i);
    else return Select(array, q + 1, r, i - k);
}

void QuickSortMoM(int *array, int l, int r) {
    if (l < r) {
        int size = r - l + 1;
        // select median as pivot
        int pivotValue = Select(array, l, r, (size + 1) / 2);
        int q = Partition(array, l, r, pivotValue);
        QuickSortMoM(array, l, q - 1);
        QuickSortMoM(array, q + 1, r);
    }
}


int main() 
{
    cin >> n;

    if(n < 40) showSteps = true;

    int array[n];
    int copy[n];
    for(int i = 0; i < n; i++)
    {
        cin >> array[i];
        if(showSteps) copy[i] = array[i];
    }

    if(showSteps) display(array);

    QuickSortMoM(array, 0, n-1);

    if(showSteps)
    {
        cout << "Input Array:\n";
        display(copy);
        cout << "Sorted Array:\n";
        display(array);
    }

    cout << "Number of comparisons: " << comparisons << "\n";

    return 0;
}