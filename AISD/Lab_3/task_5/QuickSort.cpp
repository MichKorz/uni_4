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

int partition(int *array, int low, int high) //Hoare Partition
{
    int pivot = array[low];
    int i = low-1;
    int j = high+1;

    while(true)
    {
        do{ // from left find element >= than pivot
            i++;
        } while(isBigger(pivot, array[i])); //3 1 2 5 6 7 

        do{ // from right find element <= than pivot
            j--;
        } while(isBigger(array[j], pivot)); 

        if(i >= j) break;

        swapElements(array, i, j);
    }

    if(showSteps) display(array);
    return j;
}

void QuickSort(int *array, const int l, const int h) 
{
    if (l < h)
    {
        int pivot = partition(array, l, h);
        QuickSort(array, l, pivot);
        QuickSort(array, pivot+1, h);
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

    QuickSort(array, 0, n-1);

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
    // std::ofstream file("data/qsDataWorst.txt"); //data/qsData.txt

    // if (!file) {
    //     std::cerr << "Error opening file!" << std::endl;
    //     return -1;
    // }

    // random_device rd;
    // mt19937 rng(rd());

    // int no_reps = 100;
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
    //         QuickSort(arr, 0, i-1);
    //     }
    //     auto end = high_resolution_clock::now();
    //     auto duration_us = duration_cast<microseconds>(end - start).count();
    //     file << comparisons/no_reps << " " << duration_us/no_reps << "\n";  
    // }
        
    // file.close();

    return 0;
}