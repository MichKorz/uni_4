from DataLoader import DataLoader

data_loader = DataLoader("MNIST/train_data.data", "MNIST/train_labels.data",
                         "MNIST/test_data.data", "MNIST/test_labels.data")


train_images, train_labels, test_images, test_labels = data_loader.load_data()


import torch
import torch.nn as nn
import torch.optim as optim
from torch.utils.data import DataLoader, TensorDataset
from DigitClassifier import DigitClassifier

# Assuming train_images, train_labels, test_images, test_labels are NumPy arrays
train_images = torch.tensor(train_images, dtype=torch.float32) # Normalize
train_labels = torch.tensor(train_labels, dtype=torch.float32)
test_images = torch.tensor(test_images, dtype=torch.float32)
test_labels = torch.tensor(test_labels, dtype=torch.float32)

# Reshape from (N, 784) to (N, 1, 28, 28) for CNN input
train_images = train_images.view(-1, 1, 28, 28)
test_images = test_images.view(-1, 1, 28, 28)

# Create DataLoader
train_dataset = TensorDataset(train_images, train_labels)
test_dataset = TensorDataset(test_images, test_labels)

train_loader = DataLoader(train_dataset, batch_size=64, shuffle=True)
test_loader = DataLoader(test_dataset, batch_size=64, shuffle=False)







device = torch.device("cuda" if torch.cuda.is_available() else "cpu")

model = DigitClassifier().to(device)
criterion = nn.CrossEntropyLoss()  # For multi-class classification
optimizer = optim.Adam(model.parameters(), lr=0.001)

# Training loop
num_epochs = 5
for epoch in range(num_epochs):
    model.train()
    running_loss = 0.0
    for images, labels in train_loader:
        images, labels = images.to(device), labels.to(device)

        optimizer.zero_grad()
        outputs = model(images)
        loss = criterion(outputs, labels)
        loss.backward()
        optimizer.step()

        running_loss += loss.item()

    print(f"Epoch {epoch+1}, Loss: {running_loss/len(train_loader)}")

print("Training complete.")

torch.save(model.state_dict(), "digit_classifier.pth")

model.eval()
correct = 0
total = 0

with torch.no_grad():
    for images, labels in test_loader:
        print(images, labels)
        #images, labels = images.to(device), labels.to(device)
        #outputs = model(images)
        #_, predicted = torch.max(outputs, 1)
        #total += labels.size(0)
        #correct += (predicted == labels).sum().item()

accuracy = 100 * correct / total
print(f"Test Accuracy: {accuracy:.2f}%")



