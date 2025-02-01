import socket

UDP_IP = "0.0.0.0"
UDP_PORT = 5000

# specify the socket address family (ipv4 in this case) and the socket type (UDP in this case)
sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)

# bind the socket to the address and port
sock.bind((UDP_IP, UDP_PORT))

while True:
    data, addr = sock.recvfrom(1024)
    print(data + b'0110')
