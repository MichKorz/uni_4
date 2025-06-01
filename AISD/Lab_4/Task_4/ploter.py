import pandas as pd
import matplotlib.pyplot as plt

# Read the CSV encoded in UTF-16
df = pd.read_csv("out.csv", encoding="utf-16")

# Define metrics to plot
metrics = [
    ("avg_comp", "Average Comparisons"),
    ("avg_reads", "Average Pointer Reads"),
    ("avg_writes", "Average Pointer Writes"),
    ("avg_height", "Average Tree Height")
]

# Plot and save each metric
for metric, title in metrics:
    plt.figure(figsize=(10, 6))
    for scenario in df["scenario"].unique():
        for op in ["insert", "remove"]:
            subset = df[(df["scenario"] == scenario) & (df["op"] == op)]
            label = f"{scenario}_{op}"
            plt.plot(subset["n"], subset[metric], marker='o', label=label)

    plt.title(f"{title} vs Tree Size (n)")
    plt.xlabel("Number of Elements (n)")
    plt.ylabel(title)
    plt.grid(True)
    plt.legend()
    plt.tight_layout()
    plt.savefig(f"{metric}_plot.png")
    plt.close()