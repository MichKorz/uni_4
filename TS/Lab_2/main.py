from scapy.all import IP, ICMP, send

def send_ping(destination, source_ip):
    packet = IP(dst=destination, src=source_ip) / ICMP()
    send(packet, verbose=False)
    print(f"Ping sent from {source_ip} to {destination}")

if __name__ == "__main__":
    #destination_ip = input("Enter target IP: ")
    #custom_source_ip = input("Enter custom source IP: ")
    destination_ip = "192.168.215.92"
    custom_source_ip = "192.168.215.231"
    send_ping(destination_ip, custom_source_ip)
