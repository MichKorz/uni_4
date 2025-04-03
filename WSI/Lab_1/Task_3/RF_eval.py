from DataLoader import DataLoader
import numpy as np

data_loader = DataLoader("../MNIST/train_data.data", "../MNIST/train_labels.data",
                         "../MNIST/test_data.data", "../MNIST/test_labels.data")

x_train, y_train, x_test, y_test = data_loader.load_data()

y_test = np.argmax(y_test, axis=1)



import joblib
from sklearn.metrics import accuracy_score, precision_score, recall_score

# Load the saved model
clf_loaded = joblib.load("RF_trained.pkl")

# Predict on test data
y_pred = clf_loaded.predict(x_test)

# Compute accuracy
accuracy = accuracy_score(y_test, y_pred)
precision = precision_score(y_test, y_pred, average=None)  # Weighted for class imbalance
recall = recall_score(y_test, y_pred, average=None)

print(f"Test Accuracy: {accuracy:.4f}")
print(precision)
print(recall)