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

// Partition procedure for dual-pivot quicksort using the COUNT strategy.
// It partitions the subarray A[low...high] using two pivots.
// After partitioning, the left pivot goes to index lp and the right pivot to index rp.
void DPPartition(int *array, int low, int high, int &lp, int &rp) {
    // get pivots: first and last element. p < q
    if (isBigger(array[low], array[high])) {
        swapElements(array, low, high);
    }
    int p = array[low];   // left pivot
    int q = array[high];  // right pivot

    int lt = low + 1;   // pointer for region less than p
    int gt = high - 1;  // pointer for region greater than q
    int i = low + 1;    // current element index

    // Counters for already classified elements:
    int countSmall = 0; // number of elements < p
    int countLarge = 0; // number of elements > q

    while (i <= gt) {
        // Decide comparison order based on counts
        if (countLarge > countSmall) {
            if (isBigger(array[i], q)) { // compare with bigger element first
                swapElements(array, i, gt);
                countLarge++;
                gt--;
                // Do not increment i since the swapped-in element must be examined.
                continue; //go through ifs again to check array[i]
            }
            if (isBigger(p, array[i])) { // otherwise compare with p.
                swapElements(array, i, lt);
                countSmall++;
                lt++;
            }
            // Else array[i] belongs to the middle section.
            i++;
        } else {
            if (isBigger(p, array[i])) { // compare with smaller element first
                swapElements(array, i, lt);
                countSmall++;
                lt++;
                i++;
                continue;
            }
            if (isBigger(array[i], q)) { // Otherwise compare with q.
                swapElements(array, i, gt);
                countLarge++;
                gt--;
                continue;
            }
            i++; // Otherwise, element is in the middle.
        }
    }
    // Place pivots into their correct positions.
    lt--; gt++;
    swapElements(array, low, lt);
    swapElements(array, high, gt);

    // Return pivot positions.
    lp = lt;
    rp = gt;
    
    if(showSteps) display(array);
}

void DPQuickSort(int *array, int low, int high) {
    if (low < high) {
        int lp, rp;
        DPPartition(array, low, high, lp, rp);
        
        DPQuickSort(array, low, lp - 1);
        DPQuickSort(array, lp + 1, rp - 1);
        DPQuickSort(array, rp + 1, high);
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

    DPQuickSort(array, 0, n-1);

    if(showSteps)
    {
        cout << "Input Array:\n";
        display(copy);
        cout << "Sorted Array:\n";
        display(array);
    }

    cout << "Number of comparisons: " << comparisons << "\n";

    for(int i = 0; i < n-1; i++)
    {
        if(array[i] > array[i+1])
        {
            cout << "Array is not sorted.\n";
            return -1;
        }
    }

    cout << "Array is indeed sorted\n";

    
    //CODE FOR SAVING DATA
    // showSteps = false;
    // std::ofstream file("data/dpqsDataWorst.txt"); //data/qsData.txt

    // if (!file) {
    //     std::cerr << "Error opening file!" << std::endl;
    //     return -1;
    // }

    // random_device rd;
    // mt19937 rng(rd());

    // int no_reps = 50;
    // for(int i = 100; i <= 10000; i += 100)
    // {
    //     int arr[i];
    //     uniform_int_distribution<int> bin(0, 2*i-1);
    //     // time the sort
    //     auto start = high_resolution_clock::now();
    //     comparisons = 0;
    //     for(int k = 0; k < no_reps; k++) // repeat 100 times
    //     {
    //         for (int j = 0; j < i; j++) {
    //             arr[j] = j; //bin(rng);
    //         }
    //         DPQuickSort(arr, 0, i-1);
    //     }
    //     auto end = high_resolution_clock::now();
    //     auto duration_us = duration_cast<microseconds>(end - start).count();
    //     file << comparisons/no_reps << " " << duration_us/no_reps << "\n";  
    // }
        
    // file.close();

    return 0;
}