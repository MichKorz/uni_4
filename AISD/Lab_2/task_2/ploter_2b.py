import subprocess
import matplotlib.pyplot as plt
import os

# List of number generators
RNGs = [
    "unsorted"
]
RNGs_path = "../RNGs/"

# List of sorting algorithms
sorting_algorithms = [
    "QuickSort", 
    "HybridSort",
    "DPQuickSort"
]
sorting_algorithms_path = "../task_1&4/"

# Ensure the plots directory exists
os.makedirs("./plots_2b", exist_ok=True)

# Simple loop to set variable k to 1, 10, and 100
for k in [1, 10, 100]:

    # Loop over each RNG
    for rng in RNGs:
        
        # Create a dictionary to store averages for each n
        averages_data = {}

        # Loop over n values from 10 to 50, stepping by 10
        for n in range(1000, 50001, 1000):
            
            # Create the path for the RNG command to run
            rng_command = [RNGs_path + rng, str(n)]
            
            # Initialize the accumulators for comparisons and swaps for each sorting algorithm
            comparisons = {algo: 0 for algo in sorting_algorithms}
            swaps = {algo: 0 for algo in sorting_algorithms}

            # Run the RNG k times
            for _ in range(k):
                # Run the RNG command and capture the output (n followed by n random numbers)
                rng_output = subprocess.check_output(rng_command, text=True)
                
                # For each sorting algorithm, run it with the RNG output (no data capture yet)
                for sort_algo in sorting_algorithms:
                    # Construct the path to the sorting algorithm
                    sort_command = [sorting_algorithms_path + sort_algo]
                    
                    # Feed RNG output to the sorting algorithm
                    result = subprocess.run(sort_command, input=rng_output, text=True, capture_output=True)

                    # The output should consist of two lines: comparisons and swaps
                    if result.returncode == 0:
                        # Parse the output assuming it's in the format:
                        # line 1: number_of_comparisons
                        # line 2: number_of_swaps
                        lines = result.stdout.splitlines()
                        try:
                            num_comparisons = int(lines[0])
                            num_swaps = int(lines[1])

                            # Accumulate the comparisons and swaps
                            comparisons[sort_algo] += num_comparisons
                            swaps[sort_algo] += num_swaps
                        except (IndexError, ValueError):
                            print(f"Error parsing output from {sort_algo}")
            
            # Store the average comparisons and swaps for each sorting algorithm for this value of n
            avg_comparisons = {algo: comparisons[algo] / k for algo in sorting_algorithms}
            avg_swaps = {algo: swaps[algo] / k for algo in sorting_algorithms}
            
            averages_data[n] = {
                'avg_comparisons': avg_comparisons,
                'avg_swaps': avg_swaps
            }
        
        # Create the plots for the comparisons and swaps
        # Comparisons vs n plot
        plt.figure(figsize=(10, 5))  # Create a new figure for the plot
        for sort_algo in sorting_algorithms:
            n_values = list(averages_data.keys())
            comparisons_values = [averages_data[n]['avg_comparisons'][sort_algo] for n in n_values]
            plt.plot(n_values, comparisons_values, label=f"{sort_algo} - Comparisons")

        plt.xlabel('Array size (n)')
        plt.ylabel('Average Comparisons')
        plt.title(f"Average Comparisons vs n for RNG: {rng} (k={k})")
        plt.legend()

        # Save the plot with a dynamic filename
        comparisons_filename = f"./plots_2b/k{k}_{rng}_comparisons_maxn50000.png"
        plt.savefig(comparisons_filename)
        plt.close()

        # Swaps vs n plot
        plt.figure(figsize=(10, 5))  # Create a new figure for the plot
        for sort_algo in sorting_algorithms:
            n_values = list(averages_data.keys())
            swaps_values = [averages_data[n]['avg_swaps'][sort_algo] for n in n_values]
            plt.plot(n_values, swaps_values, label=f"{sort_algo} - Swaps")

        plt.xlabel('Array size (n)')
        plt.ylabel('Average Swaps')
        plt.title(f"Average Swaps vs n for RNG: {rng} (k={k})")
        plt.legend()

        # Save the plot with a dynamic filename
        swaps_filename = f"./plots_2b/k{k}_{rng}_swaps_maxn50000.png"
        plt.savefig(swaps_filename)
        plt.close()

        # Create the new plots for average comparisons divided by n vs n
        plt.figure(figsize=(10, 5))  # Create a new figure for the plot
        for sort_algo in sorting_algorithms:
            n_values = list(averages_data.keys())
            comparisons_divided_by_n = [averages_data[n]['avg_comparisons'][sort_algo] / n for n in n_values]
            plt.plot(n_values, comparisons_divided_by_n, label=f"{sort_algo} - Comparisons / n")

        plt.xlabel('Array size (n)')
        plt.ylabel('Average Comparisons / n')
        plt.title(f"Average Comparisons / n vs n for RNG: {rng} (k={k})")
        plt.legend()

        # Save the plot with a dynamic filename for comparisons / n
        comparisons_dn_filename = f"./plots_2b/k{k}_{rng}_comparisonsDn_maxn50000.png"
        plt.savefig(comparisons_dn_filename)
        plt.close()

        # Create the new plots for average swaps divided by n vs n
        plt.figure(figsize=(10, 5))  # Create a new figure for the plot
        for sort_algo in sorting_algorithms:
            n_values = list(averages_data.keys())
            swaps_divided_by_n = [averages_data[n]['avg_swaps'][sort_algo] / n for n in n_values]
            plt.plot(n_values, swaps_divided_by_n, label=f"{sort_algo} - Swaps / n")

        plt.xlabel('Array size (n)')
        plt.ylabel('Average Swaps / n')
        plt.title(f"Average Swaps / n vs n for RNG: {rng} (k={k})")
        plt.legend()

        # Save the plot with a dynamic filename for swaps / n
        swaps_dn_filename = f"./plots_2b/k{k}_{rng}_swapsDn_maxn50000.png"
        plt.savefig(swaps_dn_filename)
        plt.close()
