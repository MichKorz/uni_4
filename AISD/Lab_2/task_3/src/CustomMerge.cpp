#include <iostream>
#include <vector>
using namespace std;

// Global counters for comparisons and swaps
int comparisons = 0;
int swaps = 0;

// Function to merge two sorted runs
vector<int> merge_runs(const vector<int>& run1, const vector<int>& run2) {
    vector<int> merged;
    size_t i = 0, j = 0;

    // Merge the two runs
    while (i < run1.size() && j < run2.size()) {
        comparisons++; // Counting comparison
        if (run1[i] <= run2[j]) {
            merged.push_back(run1[i++]);
        } else {
            merged.push_back(run2[j++]);
        }
        swaps++; // Every push_back is a swap/move
    }

    // Append any remaining elements from run1
    while (i < run1.size()) {
        merged.push_back(run1[i++]);
        swaps++;
    }

    // Append any remaining elements from run2
    while (j < run2.size()) {
        merged.push_back(run2[j++]);
        swaps++;
    }

    return merged;
}

// Function to merge all ascending runs
vector<int> merge_all_runs(vector<vector<int>> ascending_runs) {
    while (ascending_runs.size() > 1) {
        vector<vector<int>> new_runs;
        
        for (size_t i = 0; i < ascending_runs.size(); i += 2) {
            if (i + 1 < ascending_runs.size()) {
                new_runs.push_back(merge_runs(ascending_runs[i], ascending_runs[i + 1]));
            } else {
                new_runs.push_back(ascending_runs[i]); // Carry over last run if odd count
            }
        }
        
        ascending_runs = new_runs;
    }
    return ascending_runs.front();
}

int main() {
    int n;
    cin >> n;

    vector<int> numbers(n);
    for (int i = 0; i < n; i++) {
        cin >> numbers[i];
    }

    vector<vector<int>> ascending_runs;

    if (!numbers.empty()) {
        vector<int> current_run;
        current_run.push_back(numbers[0]);
        int last_run_length = 0;

        for (int i = 1; i < n; i++) {
            comparisons++; // Comparing elements for ascending order
            if (numbers[i] > numbers[i - 1]) 
            {
                current_run.push_back(numbers[i]);
            } else 
            {
                if (last_run_length == current_run.size()) 
                {
                    vector<int> last_run = ascending_runs.back();
                    ascending_runs.pop_back();
                    current_run = merge_runs(last_run, current_run);
                }
                last_run_length = current_run.size();
                ascending_runs.push_back(current_run);
                current_run.clear();
                current_run.push_back(numbers[i]);
            }
        }
        ascending_runs.push_back(current_run);
    }

    vector<int> sorted_list = merge_all_runs(ascending_runs);

    /*for (int i = 0; i < sorted_list.size(); i++) 
    {
        cout << sorted_list[i] << " ";
    }
    cout << endl;*/

    // Output comparisons and swaps
    cout << comparisons << endl;
    cout << swaps << endl;

    return 0;
}
