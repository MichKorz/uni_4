from DataLoader import DataLoader
import numpy as np

data_loader = DataLoader("../MNIST/train_data.data", "../MNIST/train_labels.data",
                         "../MNIST/test_data.data", "../MNIST/test_labels.data")


train_images, train_labels, test_images, test_labels = data_loader.load_data()
test_labels = np.argmax(test_labels, axis=1)


import torch
from torch.utils.data import DataLoader, TensorDataset
from Task_1.CNN_model import DigitClassifier

test_images = torch.tensor(test_images, dtype=torch.float32)
test_labels = torch.tensor(test_labels, dtype=torch.float32)

# Reshape from (N, 784) to (N, 1, 28, 28) for CNN input
test_images = test_images.view(-1, 1, 28, 28)

# Create DataLoader
test_dataset = TensorDataset(test_images, test_labels)

test_loader = DataLoader(test_dataset, batch_size=64, shuffle=False)

model = DigitClassifier()
model.load_state_dict(torch.load("CNN_trained.pth"))
device = torch.device("cuda" if torch.cuda.is_available() else "cpu")
model.to(device)
model.eval()

model.eval()
correct = 0
total = 0

with torch.no_grad():
    for images, labels in test_loader:
        #print(images, labels)
        images, labels = images.to(device), labels.to(device)
        outputs = model(images)
        _, predicted = torch.max(outputs, 1)
        total += labels.size(0)
        correct += (predicted == labels).sum().item()

accuracy = 100 * correct / total
print(f"Test Accuracy: {accuracy:.2f}%")

from sklearn.metrics import accuracy_score, precision_score, recall_score

# Get predictions and true labels
all_preds = []
all_labels = []

with torch.no_grad():
    for images, labels in test_loader:
        outputs = model(images)  # Forward pass
        _, preds = torch.max(outputs, 1)  # Get predicted class
        all_preds.extend(preds.cpu().numpy())
        all_labels.extend(labels.cpu().numpy())

# Compute metrics
accuracy = accuracy_score(all_labels, all_preds)
precision = precision_score(all_labels, all_preds, average=None)  # Weighted for class imbalance
recall = recall_score(all_labels, all_preds, average=None)

print(f"Accuracy: {accuracy:.4f}")
print(precision)
print(recall)