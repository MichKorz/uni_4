import numpy as np
import matplotlib.pyplot as plt
from sklearn.cluster import KMeans
from sklearn.preprocessing import StandardScaler
from sklearn.datasets import fetch_openml
from collections import Counter
import seaborn as sns

# Load EMNIST MNIST dataset
def load_data():
    mnist = fetch_openml('mnist_784', version=1, as_frame=False)
    data = mnist.data
    labels = mnist.target.astype(int)
    return data, labels

# Run KMeans clustering with smart centroid init and return best model (lowest inertia)
def run_kmeans(data, n_clusters, n_init=10, attempts=5):
    best_inertia = np.inf
    best_model = None
    for _ in range(attempts):
        model = KMeans(n_clusters=n_clusters, init='k-means++', n_init=n_init, random_state=None)
        model.fit(data)
        if model.inertia_ < best_inertia:
            best_inertia = model.inertia_
            best_model = model
    return best_model

# Build and plot cluster-to-digit percentage matrix
def plot_cluster_digit_matrix(model, labels, n_clusters):
    cluster_labels = model.labels_
    confusion = np.zeros((n_clusters, 10), dtype=int)
    for cluster_id in range(n_clusters):
        indices = np.where(cluster_labels == cluster_id)[0]
        digits_in_cluster = labels[indices]
        count = Counter(digits_in_cluster)
        for digit in range(10):
            confusion[cluster_id][digit] = count[digit]
    percentages = confusion / confusion.sum(axis=1, keepdims=True) * 100

    plt.figure(figsize=(10, 8))
    sns.heatmap(percentages, annot=True, fmt=".1f", cmap='Blues', xticklabels=range(10), yticklabels=range(n_clusters))
    plt.xlabel("Digit")
    plt.ylabel("Cluster")
    plt.title(f"Digit Distribution in {n_clusters} Clusters (%)")
    plt.show()

# Show centroid images
def plot_centroids(model):
    centroids = model.cluster_centers_
    plt.figure(figsize=(10, 10))
    for i in range(len(centroids)):
        plt.subplot(int(np.sqrt(len(centroids))), int(np.ceil(len(centroids) / np.sqrt(len(centroids)))), i + 1)
        plt.imshow(centroids[i].reshape(28, 28), cmap='gray')
        plt.axis('off')
    plt.suptitle("Cluster Centroids as Images")
    plt.show()

# Full pipeline
def run_all(n_clusters_list):
    data, labels = load_data()
    data_scaled = StandardScaler().fit_transform(data)

    for n_clusters in n_clusters_list:
        print(f"\n=== Clustering for k = {n_clusters} ===")
        model = run_kmeans(data_scaled, n_clusters=n_clusters, attempts=5)
        plot_cluster_digit_matrix(model, labels, n_clusters)
        plot_centroids(model)

# Run for 10, 15, 20, 30 clusters
run_all([10, 15, 20, 30])
