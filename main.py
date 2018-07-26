import socket
import sys

port = int(sys.argv[1])

data = 'this is fun!'
size = len(data)

s = socket.socket()
s.connect(('localhost', port))
s.send(socket.htonl(size).to_bytes(4, sys.byteorder))
print(s.recv(10).decode())

exit(0)
