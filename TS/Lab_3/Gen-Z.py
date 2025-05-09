import random

# Length of the generated message
message_length = 100  # Change this value as needed

# Generate random binary string
binary_string = ''.join(random.choice('01') for _ in range(message_length))

# Write the binary string to file named 'Z'
with open('Z', 'w') as file:
    file.write(binary_string)
