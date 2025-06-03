import matplotlib.pyplot as plt
import pandas as pd

# Load the data
df = pd.read_csv('out.txt', encoding='utf-16')

# Plot
plt.figure(figsize=(10, 6))
plt.plot(df['n'], df['avg_rounds'], label='Average Rounds', marker='o')
plt.plot(df['n'], df['min_rounds'], label='Min Rounds', linestyle='--', marker='x')
plt.plot(df['n'], df['max_rounds'], label='Max Rounds', linestyle='--', marker='^')

# Labels and title
plt.title('Information Propagation Rounds in Tree')
plt.xlabel('Number of Nodes')
plt.ylabel('Rounds')
plt.grid(True)
plt.legend()
plt.tight_layout()

# Save to file
plt.savefig('rounds_plot.png')