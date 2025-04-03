import os
from PIL import Image
import torchvision.transforms as transforms
import torch

# Path to the folder containing images
folder_path = "Images/"

from Task_1.CNN_model import DigitClassifier

model = DigitClassifier()
model.load_state_dict(torch.load("../Task_1/CNN_trained.pth"))
device = torch.device("cuda" if torch.cuda.is_available() else "cpu")
model.to(device)

# Load model (ensure it's already defined and loaded)
model.eval()

# Define the same transformations
transform = transforms.Compose([
    transforms.ToTensor(),
    transforms.Normalize((0.5,), (0.5,))
])

# Loop through all PNG files in the folder
for file_name in os.listdir(folder_path):
    if file_name.endswith(".png"):  # Process only PNG files
        file_path = os.path.join(folder_path, file_name)

        # Load and process the image
        image = Image.open(file_path).convert("L")
        image_tensor = transform(image).unsqueeze(0)  # Add batch dimension

        # Make prediction
        with torch.no_grad():
            output = model(image_tensor)
            predicted_label = torch.argmax(output, dim=1).item()

        print(f"{file_name}: Predicted digit = {predicted_label}")