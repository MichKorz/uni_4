from DataLoader import DataLoader

data_loader = DataLoader("MNIST/train_data.data", "MNIST/train_labels.data",
                         "MNIST/test_data.data", "MNIST/test_labels.data")


train_images, train_labels, test_images, test_labels = data_loader.load_data()


import torch
from torch.utils.data import DataLoader, TensorDataset
from DigitClassifier import DigitClassifier

# Assuming train_images, train_labels, test_images, test_labels are NumPy arrays
test_images = torch.tensor(test_images, dtype=torch.float32)
test_labels = torch.tensor(test_labels, dtype=torch.long)


# Reshape from (N, 784) to (N, 1, 28, 28) for CNN input
test_images = test_images.view(-1, 1, 28, 28)

# Create DataLoader
test_dataset = TensorDataset(test_images, test_labels)

test_loader = DataLoader(test_dataset, batch_size=100, shuffle=False)

device = torch.device("cuda" if torch.cuda.is_available() else "cpu")

model = DigitClassifier()
model.load_state_dict(torch.load("digit_classifier.pth"))
model.to(device)
model.eval()

model.eval()
correct = 0
total = 0

with torch.no_grad():
    for images, labels in test_loader:
        images, labels = images.to(device), labels.to(device)
        outputs = model(images)
        _, predicted = torch.max(outputs, 1)
        labels = labels.argmax(dim=1)
        total += labels.size(0)
        correct += (predicted == labels).sum().item()

accuracy = 100 * correct / total
print(f"Test Accuracy: {accuracy:.2f}%")