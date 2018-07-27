import socket
import sys

port = int(sys.argv[1])

# vector is a list of tuples
def sendVector(vector: list):
    
    # Connect to the socket
    s = socket.socket()
    while True:
        try:
            s.connect(('localhost', port))
            break
        except ConnectionRefusedError:
            print('Error connecting to socket, trying again...')

    numPairs = len(vector)
    s.send(socket.htonl(numPairs).to_bytes(4, sys.byteorder))

    for pair in vector:
        s.send(socket.htonl(pair[0]).to_bytes(4, sys.byteorder))
        s.send(socket.htonl(pair[1]).to_bytes(4, sys.byteorder))

