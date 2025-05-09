FLAG = "01111110"
GENERATOR = "100000111"  # CRC-8

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
            if count == 5 and i + 1 < len(data) and data[i+1] == '0':
                i += 1  # skip stuffed 0
                count = 0
        else:
            count = 0
        i += 1
    return result

def compute_crc(data, generator):
    n = len(generator)
    data_padded = data + '0' * (n - 1)
    data_list = list(data_padded)
    generator = list(generator)

    for i in range(len(data)):
        if data_list[i] == '1':
            for j in range(n):
                data_list[i + j] = str(int(data_list[i + j] != generator[j]))
    return ''.join(data_list[-(n-1):])

def encode_file(in_file, encoded_file, chunk_size=64):
    with open(in_file, 'r') as f:
        bits = f.read().strip()

    frames = []
    for i in range(0, len(bits), chunk_size):
        chunk = bits[i:i+chunk_size]
        crc = compute_crc(chunk, GENERATOR)
        full_data = chunk + crc
        stuffed = bit_stuff(full_data)
        frame = FLAG + stuffed + FLAG
        frames.append(frame)

    with open(encoded_file, 'w') as f:
        f.write(''.join(frames))

    print("----- Zakodowywanie: -----")
    print(f"Liczba wszystkich ramek: {len(frames)}")

def decode_file(encoded_file, out_file):
    with open(encoded_file, 'r') as f:
        data = f.read().strip()

    raw_frames = data.split(FLAG)
    # usuń puste fragmenty lub błędne
    frames = [frame for frame in raw_frames if frame]

    total = len(frames)
    good = 0
    bad = 0
    recovered_bits = ''

    for frame in frames:
        try:
            unstuffed = bit_unstuff(frame)
            crc_len = len(GENERATOR) - 1
            payload = unstuffed[:-crc_len]
            received_crc = unstuffed[-crc_len:]
            calc_crc = compute_crc(payload, GENERATOR)
            if calc_crc == received_crc:
                recovered_bits += payload
                good += 1
            else:
                bad += 1
        except:
            bad += 1

    with open(out_file, 'w') as f:
        f.write(recovered_bits)

    print("----- Odkodowywanie: -----")
    print(f"Liczba wszystkich ramek: {total}")
    print(f"Liczba poprawnych ramek: {good}")
    print(f"Liczba błędnych ramek: {bad}")


encode_file('W', 'encoded.txt')
decode_file('encoded.txt', 'out.txt')