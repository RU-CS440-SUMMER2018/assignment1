import socket
import sys
import search

# Getting inputs from arguments
port = int(sys.argv[1])
filename = sys.argv[2]
initial_i = int(sys.argv[3])
initial_j = int(sys.argv[4])
goal_i = int(sys.argv[5])
goal_j = int(sys.argv[6])

def sendVector(vector):

    '''
    Sends <vector>, which is a list of tuples,
    to the parent c++ program
    '''
    
    # Connect to the socket
    s = socket.socket()
    while True:
        try:
            s.connect(('localhost', port))
            break
        except ConnectionRefusedError:
            print('Error connecting to socket, trying again...')

    # Calculate size of vector and let c++ know
    numPairs = len(vector)
    s.send(socket.htonl(numPairs).to_bytes(4, sys.byteorder))

    # Send the actual vector
    for pair in vector:
        s.send(socket.htonl(pair[0]).to_bytes(4, sys.byteorder))
        s.send(socket.htonl(pair[1]).to_bytes(4, sys.byteorder))

# Call the search function and send the vector
vector = search.search(filename, initial_i, initial_j, goal_i, goal_j)
sendVector(vector)
