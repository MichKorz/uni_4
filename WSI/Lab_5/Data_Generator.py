import numpy as np
from sklearn.model_selection import train_test_split

def generate_raw_data(n_samples=1000):
    X = np.random.uniform(-1, 1, size=(n_samples, 2))
    X = X[(X[:, 0] != 0) & (X[:, 1] != 0)]
    y = ((X[:, 0] * X[:, 1]) > 0).astype(int).reshape(-1, 1)
    return X, y

def l1_normalize(X):
    return X / np.sum(np.abs(X), axis=1, keepdims=True)

def l2_normalize(X):
    return X / np.linalg.norm(X, axis=1, keepdims=True)

def generate_l1_normalized_data(n_samples=1000):
    X, y = generate_raw_data(n_samples)
    X_norm = l1_normalize(X)
    return X_norm, y

def generate_l2_normalized_data(n_samples=1000):
    X, y = generate_raw_data(n_samples)
    X_norm = l2_normalize(X)
    return X_norm, y

def split_and_save_txt(X, y, prefix, test_size=0.2):
    X_train, X_test, y_train, y_test = train_test_split(X, y, test_size=test_size, random_state=42)

    np.savetxt(f"{prefix}_X_train.txt", X_train, fmt="%.6f", delimiter=",")
    np.savetxt(f"{prefix}_y_train.txt", y_train, fmt="%d")
    np.savetxt(f"{prefix}_X_test.txt", X_test, fmt="%.6f", delimiter=",")
    np.savetxt(f"{prefix}_y_test.txt", y_test, fmt="%d")

if __name__ == "__main__":
    X_raw, y_raw = generate_raw_data()
    split_and_save_txt(X_raw, y_raw, "raw")

    X_l1, y_l1 = generate_l1_normalized_data()
    split_and_save_txt(X_l1, y_l1, "l1")

    X_l2, y_l2 = generate_l2_normalized_data()
    split_and_save_txt(X_l2, y_l2, "l2")

    print("Datasets saved to comma-separated .txt files.")
