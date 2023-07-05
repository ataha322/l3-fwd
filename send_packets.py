# Import the necessary modules
import socket
import struct

# Create a raw socket
raw_socket = socket.socket(socket.AF_PACKET, socket.SOCK_RAW, socket.htons(0x0800))

# Bind the socket to the first virtual NIC
raw_socket.bind(('vnet4', 0))

# Construct the packet
packet = struct.pack('!6s6sH', b'12345678', b'87654321', 1234)

# Send the packet
raw_socket.send(packet)

