def load_data(file_path):
    """
    Expects each line like:
      comp time
    """
    comps = []
    times = []
    with open(file_path, "r") as f:
        for line in f:
            tokens = line.split(" ")
            if not tokens:
                continue

            # Last token is the average time for that line
            avg_time = float(tokens.pop())
            avg_cmp = float(tokens.pop())

            # Now tokens are [comp, swap, comp, swap, …]
            comps.append(avg_cmp)
            times.append(avg_time)

    return comps, times

def draw_lines(divide, scale, scale2, n_values):
    line = np.array(n_values)**2 / divide
    plt.plot(n_values,line,color='red', linestyle=':',linewidth=2.5,label=f"{1.0/divide:.3f}n²")

    log_curve = n_values * np.log2(n_values)
    # scale it to line up better with data
    log_curve *= scale
    plt.plot(n_values,log_curve,color='red', linestyle='--',linewidth=1.5,label=f"{scale:.2f}*log₂(N)")

    if scale2 != 0:
        log_curve = n_values * np.log2(n_values)
        # scale it to line up better with data
        log_curve *= scale2
        plt.plot(n_values,log_curve,color='red', linestyle='--',linewidth=1.5,label=f"{scale2:.2f}*log₂(N)")


file_paths = ["data/qsData.txt", "data/qsDataWorst.txt", "data/sqsData.txt", "data/sqsDataWorst.txt", "data/dpqsData.txt", "data/dpqsDataWorst.txt", "data/sdpqsData.txt", "data/sdpqsDataWorst.txt"]
titles = [
    "QS_Avg_no_comps",
    "QS_Avg_no_times",
    "DPQS_Avg_no_comps",
    "DPQS_Avg_no_times"
]
labels = ["Quick Sort - random data", "Quick Sort - worst data", "Select Quick Sort - random data", "Select Quick Sort - worst data", "Dual-Pivot Quick Sort - random data", "Dual-Pivot Quick Sort - worst data", "Select Dual-Pivot Quick Sort - random data", "Select Dual-Pivot Quick Sort - worst data"]
colors = ["magenta", "green", "blue", "black", "magenta", "green", "blue", "black"]

comps = []
times = []
for file_path in file_paths:
    c, t = load_data(file_path)
    comps.append(c)
    times.append(t)

import matplotlib.pyplot as plt
import numpy as np

n_values = list(range(100, 10001, 100))

for i in range(0, 4):
    plt.plot(n_values, comps[i], color=colors[i], linewidth=2, label=labels[i])

draw_lines(2, 3.0, 0, n_values)

plt.xlabel("Array size")
plt.ylabel("Average comparisons")
plt.title(f"Average Comparisons for 100 reps")
plt.legend()  # <-- Show legend with algorithm names
name = "plots/" + titles[0] + ".png"
plt.savefig(name)
plt.clf()

for i in range(0, 4):
    plt.plot(n_values, times[i], color=colors[i], linewidth=2, label=labels[i])

draw_lines(700, 0.08, 0.02, n_values)

plt.xlabel("Array size")
plt.ylabel("Average times")
plt.title(f"Average times for 100 reps")
plt.legend()  # <-- Show legend with algorithm names
name = "plots/" + titles[1] + ".png"
plt.savefig(name)
plt.clf()

for i in range(4, 8):
    plt.plot(n_values, comps[i], color=colors[i], linewidth=2, label=labels[i])

draw_lines(2, 3.0, 0, n_values)

plt.xlabel("Array size")
plt.ylabel("Average comparisons")
plt.title(f"Average Comparisons for 100 reps")
plt.legend()  # <-- Show legend with algorithm names
name = "plots/" + titles[2] + ".png"
plt.savefig(name)
plt.clf()

for i in range(4, 8):
    plt.plot(n_values, times[i], color=colors[i], linewidth=2, label=labels[i])

draw_lines(700, 0.02, 0.1, n_values)

plt.xlabel("Array size")
plt.ylabel("Average times")
plt.title(f"Average times for 100 reps")
plt.legend()  # <-- Show legend with algorithm names
name = "plots/" + titles[3] + ".png"
plt.savefig(name)
plt.clf()
