import numpy as np
import matplotlib.pyplot as plt
from sklearn.datasets import fetch_openml
from sklearn.preprocessing import StandardScaler
from collections import deque, Counter
from sklearn.metrics import confusion_matrix

# === Load and preprocess EMNIST MNIST ===
def load_data(n_samples=1000):
    mnist = fetch_openml('mnist_784', version=1, as_frame=False)
    data = mnist.data[:n_samples]
    labels = mnist.target.astype(int)[:n_samples]
    data = StandardScaler().fit_transform(data)
    return data, labels

# === Euclidean distance ===
def euclidean(a, b):
    return np.linalg.norm(a - b)

# === DBSCAN implementation ===
def dbscan(X, eps, min_pts):
    n = len(X)
    labels = np.full(n, -1)  # -1 means noise
    visited = np.zeros(n, dtype=bool)
    cluster_id = 0

    def region_query(idx):
        return [i for i in range(n) if euclidean(X[idx], X[i]) <= eps]

    def expand_cluster(idx, neighbors):
        nonlocal cluster_id
        labels[idx] = cluster_id
        queue = deque(neighbors)
        while queue:
            current = queue.popleft()
            if not visited[current]:
                visited[current] = True
                current_neighbors = region_query(current)
                if len(current_neighbors) >= min_pts:
                    queue.extend(current_neighbors)
            if labels[current] == -1:
                labels[current] = cluster_id

    for i in range(n):
        if visited[i]:
            continue
        visited[i] = True
        neighbors = region_query(i)
        if len(neighbors) < min_pts:
            labels[i] = -1  # noise
        else:
            expand_cluster(i, neighbors)
            cluster_id += 1
    return labels

# === Cluster evaluation ===
def evaluate_clusters(pred_labels, true_labels):
    noise_mask = pred_labels == -1
    clustered_mask = ~noise_mask

    n_noise = np.sum(noise_mask)
    total = len(true_labels)
    n_clustered = total - n_noise

    cluster_ids = np.unique(pred_labels[pred_labels != -1])
    purity_total = 0
    misclassified_total = 0

    for cluster in cluster_ids:
        indices = np.where(pred_labels == cluster)[0]
        true = true_labels[indices]
        most_common = Counter(true).most_common(1)[0]
        correct = most_common[1]
        total_in_cluster = len(true)
        purity_total += correct
        misclassified_total += total_in_cluster - correct

    purity = purity_total / n_clustered * 100
    noise_percent = n_noise / total * 100
    misclassified_percent = misclassified_total / n_clustered * 100

    return purity, noise_percent, misclassified_percent

from sklearn.manifold import TSNE
import matplotlib.pyplot as plt

def visualize_clusters(X, labels):
    print("Reducing dimensionality for visualization...")
    X_2d = TSNE(n_components=2, init='random', random_state=42).fit_transform(X)

    plt.figure(figsize=(10, 8))
    unique_labels = np.unique(labels)

    for lbl in unique_labels:
        mask = labels == lbl
        if lbl == -1:
            plt.scatter(X_2d[mask, 0], X_2d[mask, 1], s=10, c='black', label='Noise', alpha=0.3)
        else:
            plt.scatter(X_2d[mask, 0], X_2d[mask, 1], s=10, label=f'Cluster {lbl}')

    plt.legend(loc='best', markerscale=2)
    plt.title("DBSCAN Clusters (via t-SNE)")
    plt.tight_layout()
    plt.show()

# === Main run ===
def run_dbscan():
    X, y = load_data(n_samples=1000)

    eps = 5.0    # Tunable parameter
    min_pts = 4   # Tunable parameter

    print("Running DBSCAN...")
    labels = dbscan(X, eps, min_pts)

    purity, noise_pct, misclassified_pct = evaluate_clusters(labels, y)
    n_clusters = len(set(labels)) - (1 if -1 in labels else 0)

    print(f"Clusters found: {n_clusters}")
    print(f"Purity: {purity:.2f}%")
    print(f"Noise: {noise_pct:.2f}%")
    print(f"Misclassified inside clusters: {misclassified_pct:.2f}%")
    visualize_clusters(X, labels)

run_dbscan()
