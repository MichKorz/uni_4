import matplotlib.pyplot as plt

def read_data(filename):
    with open(filename, 'r', encoding='utf-16') as f:
        lines = f.readlines()

    n_vals = []
    prim_times = []
    kruskal_times = []

    for line in lines[1:]:  # skip header
        parts = line.strip().split()
        if len(parts) != 3:
            continue
        n, prim, kruskal = parts
        n_vals.append(int(n))
        prim_times.append(float(prim))
        kruskal_times.append(float(kruskal))

    return n_vals, prim_times, kruskal_times

def plot_times(n_vals, prim_times, kruskal_times, output_file):
    plt.figure(figsize=(10, 6))
    plt.plot(n_vals, prim_times, marker='o', label='Prim')
    plt.plot(n_vals, kruskal_times, marker='s', label='Kruskal')
    plt.xlabel("Number of vertices (n)")
    plt.ylabel("Average execution time [ms]")
    plt.title("MST Algorithm Performance: Prim vs Kruskal")
    plt.legend()
    plt.grid(True)
    plt.tight_layout()
    plt.savefig(output_file)
    plt.close()

if __name__ == "__main__":
    input_file = "out.txt"      
    output_file = "mst_comparison.png" 

    n_vals, prim_times, kruskal_times = read_data(input_file)
    plot_times(n_vals, prim_times, kruskal_times, output_file)
