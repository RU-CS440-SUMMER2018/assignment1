import socket
import sys

port = int(sys.argv[1])

data = 'this is fun!'
size = len(data)

s = socket.socket()

while True:
    try:
        s.connect(('localhost', port))
        break
    except ConnectionRefusedError:
        print('Error connecting to socket, trying again...')

s.send(socket.htonl(size).to_bytes(4, sys.byteorder))
print(s.recv(10).decode())

exit(0)
