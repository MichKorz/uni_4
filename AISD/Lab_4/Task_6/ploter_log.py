import pandas as pd
import matplotlib.pyplot as plt

# Read UTF-16 encoded CSV
df = pd.read_csv("out.csv", encoding="utf-8")

# Metrics and titles
metrics = [
    ("avg_comp", "Average Comparisons"),
    ("avg_reads", "Average Pointer Reads"),
    ("avg_writes", "Average Pointer Writes"),
    ("avg_height", "Average Tree Height")
]

# Generate log-scale plots
for metric, title in metrics:
    plt.figure(figsize=(10, 6))
    for scenario in df["scenario"].unique():
        for op in ["insert", "remove"]:
            subset = df[(df["scenario"] == scenario) & (df["op"] == op)]
            label = f"{scenario}_{op}"
            plt.plot(subset["n"], subset[metric], marker='o', label=label)

    plt.title(f"{title} vs Tree Size (n) [Log Scale]")
    plt.xlabel("Number of Elements (n)")
    plt.ylabel(title)
    plt.yscale("log")
    plt.grid(True, which="both", linestyle='--', linewidth=0.5)
    plt.legend()
    plt.tight_layout()
    plt.savefig(f"{metric}_log_plot.png")
    plt.close()