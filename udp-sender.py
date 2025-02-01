import socket
import struct


UDP_IP = "127.0.0.1" #target ip
UDP_RECEIVER_PORT = 5000


# Format characters for struct module (format char, description, size in bytes):
#
# x  : pad byte (no value)
# c  : char (bytes of length 1) 1
# b  : signed char (integer) 1
# B  : unsigned char (integer) 1
# ?  : _Bool (bool) 1
# h  : short (integer) 2
# H  : unsigned short (integer) 2
# i  : int (integer) 4
# I  : unsigned int (integer) 4
# l  : long (integer) 4
# L  : unsigned long (integer) 4
# q  : long long (integer) 8
# Q  : unsigned long long (integer) 8

# ! to specfiy big endian

DATA = [1, 2, 3, 4, 5, 6, 7, 8, 9, 10]

DATA_BYTES = struct.pack("!HHHHHHHHHH", *DATA)

# specify the socket address family (ipv4 in this case) and the socket type (UDP in this case)
sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)

# send the message to the receiver
sock.sendto(DATA_BYTES, (UDP_IP, UDP_RECEIVER_PORT))
print(f"Sent: {struct.unpack('!HHHHHHHHHH', DATA_BYTES)}")
