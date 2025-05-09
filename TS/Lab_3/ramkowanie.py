FRAME_SIZE = 32
FLAG = "01111110"
#CRC_GENERATOR = "1101"


# CRC-8 function with polynomial 0x07 (x^8 + x^2 + x + 1)
def crc8(data: str) -> int:
    crc = 0
    for bit in data:
        crc ^= int(bit) << 7  # Align to the leftmost bit
        for _ in range(8):
            if crc & 0x80:
                crc = ((crc << 1) ^ 0x07) & 0xFF
            else:
                crc = (crc << 1) & 0xFF
    return crc

# Function to handle the insertion of a "0" after five consecutive "1"s
def insert_zero_after_five_consecutive_ones(chunk: str) -> str:
    result = []
    count = 0

    # Loop through each bit and detect five consecutive '1's
    for i in range(len(chunk)):
        result.append(chunk[i])
        if chunk[i] == '1':
            count += 1
        else:
            count = 0

        # If there are five consecutive '1's, insert a '0' after the last '1'
        if count == 5:
            result.append('0')  # Add a '0' after the last '1' in the sequence
            count = 0  # Reset the count after inserting the '0'

    # Return the modified chunk as a string
    return ''.join(result)

# Function to add flags and save the data to file
def add_flags_and_save(chunks_with_crc):
    flag = '01111110'  # Flag to be added at the start and end of each chunk
    # Open file W to write the output
    with open('W', 'w') as file:
        for chunk in chunks_with_crc:
            # Add flag to the start and end of each chunk and write to the file
            chunk_with_flags = flag + chunk + flag
            file.write(chunk_with_flags)


# Read binary string from file 'Z'
with open('Z', 'r') as file:
    binary_string = file.read().strip()

# Split into chunks of 32 characters
chunks = [binary_string[i:i+FRAME_SIZE] for i in range(0, len(binary_string), FRAME_SIZE)]

# Compute CRC-8, append it as 8-bit binary, and collect the results
chunks_with_crc = []
for chunk in chunks:
    crc = crc8(chunk)
    crc_bin = f'{crc:08b}'  # Convert CRC to 8-bit binary string
    chunk_with_crc = chunk + crc_bin
    chunks_with_crc.append(chunk_with_crc)

# Modify chunks by inserting a '0' after five consecutive '1's
chunks_with_modified_crc = [insert_zero_after_five_consecutive_ones(chunk) for chunk in chunks_with_crc]

# Add flags and save the data to file 'W'
add_flags_and_save(chunks_with_modified_crc)