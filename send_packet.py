from scapy.all import *

dst_mac = "00:11:22:33:44:55"  # Replace with the destination MAC address
src_mac = "aa:bb:cc:dd:ee:ff"  # Replace with the source MAC address

pkt = Ether(dst=dst_mac, src=src_mac) / IP(dst="192.168.3.1", src="192.168.0.2") / TCP()  # Customize the packet as needed

for _ in range(3):
    sendp(pkt, iface="dtap0")
