FLAG = "01111110"
GENERATOR = "100000111"  # CRC-8 polynomial

def bit_stuff(data):
    count = 0
    result = ""
    for bit in data:
        result += bit
        if bit == '1':
            count += 1
            if count == 5:
                result += '0'
                count = 0
        else:
            count = 0
    return result

def bit_unstuff(data):
    count = 0
    result = ""
    i = 0
    while i < len(data):
        bit = data[i]
        result += bit
        if bit == '1':
            count += 1
            if count == 5 and i + 1 < len(data) and data[i + 1] == '0':
                i += 1  # skip the stuffed '0'
                count = 0
        else:
            count = 0
        i += 1
    return result

def compute_crc(data, generator):
    n = len(generator)
    padded_data = data + '0' * (n - 1)
    data_list = list(padded_data)
    generator = list(generator)

    for i in range(len(data)):
        if data_list[i] == '1':
            for j in range(n):
                data_list[i + j] = str(int(data_list[i + j] != generator[j]))
    return ''.join(data_list[-(n - 1):])  # return the CRC remainder

def encode_file(in_file, encoded_file, chunk_size=64):
    with open(in_file, 'r') as f:
        bits = f.read().strip()

    frames = []
    for i in range(0, len(bits), chunk_size):
        chunk = bits[i:i + chunk_size]
        crc = compute_crc(chunk, GENERATOR)
        data_with_crc = chunk + crc
        stuffed = bit_stuff(data_with_crc)
        frame = FLAG + stuffed + FLAG
        frames.append(frame)

    with open(encoded_file, 'w') as f:
        f.write(''.join(frames))

    print("----- Encoding -----")
    print(f"Total frames: {len(frames)}")

def decode_file(encoded_file, out_file):
    with open(encoded_file, 'r') as f:
        data = f.read().strip()

    raw_frames = data.split(FLAG)
    frames = [frame for frame in raw_frames if frame]

    total = len(frames)
    valid = 0
    invalid = 0
    recovered_bits = ''

    for frame in frames:
        try:
            unstuffed = bit_unstuff(frame)
            crc_len = len(GENERATOR) - 1
            payload = unstuffed[:-crc_len]
            received_crc = unstuffed[-crc_len:]
            calculated_crc = compute_crc(payload, GENERATOR)
            if calculated_crc == received_crc:
                recovered_bits += payload
                valid += 1
            else:
                invalid += 1
        except:
            invalid += 1

    with open(out_file, 'w') as f:
        f.write(recovered_bits)

    print("----- Decoding -----")
    print(f"Total frames: {total}")
    print(f"Valid frames: {valid}")
    print(f"Invalid frames: {invalid}")

# Example usage:
encode_file('Z', 'encoded.txt')
decode_file('encoded.txt', 'out.txt')
