from scapy.all import IP, ICMP, sr1

def send_standard_ping(target_ip):
    packet = IP(dst=target_ip) / ICMP()
    response = sr1(packet, timeout=2, verbose=False)

    if response:
        print(f"Reply from {response.src}: ICMP type {response.type}")
    else:
        print("No response received.")

if __name__ == "__main__":
    #target = input("Enter target IP: ")
    target = "192.168.215.231"
    send_standard_ping(target)