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


void DPQuickSortMoM(int *array, int low, int high) {
    if (low < high) {
        int length = high - low + 1;
        // compute 1/3 and 2/3 positions (1-based)
        int firstPos = static_cast<int>(std::ceil(length / 3.0));
        int secondPos = static_cast<int>(std::ceil(2 * length / 3.0));

        int pVal = Select(array, low, high, firstPos);
        int qVal = Select(array, low, high, secondPos);
        if (isBigger(pVal, qVal)) std::swap(pVal, qVal);

        // move pVal to low
        int idxP = low;
        while (idxP <= high && array[idxP] != pVal) ++idxP;
        if (idxP > high) idxP = low;
        swapElements(array, idxP, low);

        // move qVal to high
        int idxQ = high;
        while (idxQ >= low && array[idxQ] != qVal) --idxQ;
        if (idxQ < low) idxQ = high;
        swapElements(array, idxQ, high);

        int lp, rp;
        DPPartition(array, low, high, lp, rp);
        DPQuickSortMoM(array, low, lp - 1);
        DPQuickSortMoM(array, lp + 1, rp - 1);
        DPQuickSortMoM(array, rp + 1, high);
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

    DPQuickSortMoM(array, 0, n-1);

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