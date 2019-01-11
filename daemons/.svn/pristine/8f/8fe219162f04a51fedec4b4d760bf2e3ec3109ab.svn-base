import sys
import socket

if(len(sys.argv) != 3):
    print "Usage: python listenUdp.py 127.0.0.1 7086"
    exit(1)
UDP_IP = sys.argv[1]
UDP_PORT = int(sys.argv[2])

sock = socket.socket(socket.AF_INET, # Internet
                     socket.SOCK_DGRAM) # UDP
sock.bind((UDP_IP, UDP_PORT))

while True:
    data, addr = sock.recvfrom(1024) # buffer size is 1024 bytes
    print data
