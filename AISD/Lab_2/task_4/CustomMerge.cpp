#include <iostream>
#include <vector>
#include <stdlib.h>
using namespace std;


vector<int> merge_runs(const vector<int>& run1, const vector<int>& run2) {
    vector<int> merged;
    size_t i = 0, j = 0;
    
    // Merge the two runs
    while (i < run1.size() && j < run2.size()) {
        if (run1[i] <= run2[j]) {
            merged.push_back(run1[i++]);
        } else {
            merged.push_back(run2[j++]);
        }
    }
    
    // Append any remaining elements from run1
    while (i < run1.size()) {
        merged.push_back(run1[i++]);
    }
    
    // Append any remaining elements from run2
    while (j < run2.size()) {
        merged.push_back(run2[j++]);
    }
    
    return merged;
}

vector<int> merge_all_runs(vector<vector<int>> ascending_runs) {
    // Continue merging until only one run remains
    while (ascending_runs.size() > 1) {
        vector<vector<int>> new_runs;
        // Merge adjacent pairs: run 0 & 1, 2 & 3, etc.
        for (size_t i = 0; i < ascending_runs.size(); i += 2) {
            if (i + 1 < ascending_runs.size()) {
                new_runs.push_back(merge_runs(ascending_runs[i], ascending_runs[i + 1]));
            } else {
                // If there's an odd number of runs, just carry the last one over
                new_runs.push_back(ascending_runs[i]);
            }
        }
        // Replace the list of runs with the merged runs
        ascending_runs = new_runs;
    }
    // Return the final merged (and sorted) run
    return ascending_runs.front();
}

int main() 
{
    int n;
    cin >> n;

    // Read n numbers from standard input
    vector<int> numbers(n);
    for (int i = 0; i < n; i++) 
    {
        cin >> numbers[i];
    }

    // Vector to store ascending runs
    vector<vector<int>> ascending_runs;
    
    if (!numbers.empty()) 
    {
        // Start the first run with the first number
        vector<int> current_run;
        current_run.push_back(numbers[0]);

        int last_run_length = 0;
        
        // Loop through the rest of the numbers
        for (int i = 1; i < n; i++) {
            // If the current number is greater than the previous, continue the run
            if (numbers[i] > numbers[i - 1]) 
            {
                current_run.push_back(numbers[i]);
            } 
            else 
            {
                // Otherwise, save the current run and start a new run
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
        // Push the final run
        ascending_runs.push_back(current_run);
    }

    // Optionally, output the ascending runs
    for (const auto& run : ascending_runs) 
    {
        for (int num : run) 
        {
            cout << num << " ";
        }
        cout << endl;
    }

    vector<int> sorted_list = merge_all_runs(ascending_runs);

    for (int i = 0; i < sorted_list.size(); i++)
    {
        std::cout << sorted_list[i] << " ";
    }

    std::cout << endl;

    return 0;
}