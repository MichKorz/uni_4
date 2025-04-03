from sklearn.ensemble import RandomForestClassifier
from DataLoader import DataLoader
import numpy as np

data_loader = DataLoader("../MNIST/train_data.data", "../MNIST/train_labels.data",
                         "../MNIST/test_data.data", "../MNIST/test_labels.data")


x_train, y_train, x_test, y_test = data_loader.load_data()

y_train = np.argmax(y_train, axis=1)
y_test = np.argmax(y_test, axis=1)


clf = RandomForestClassifier(n_estimators=100, random_state=42)
clf.fit(x_train, y_train)

import joblib

# Save the model
joblib.dump(clf, "RF_trained.pkl")
