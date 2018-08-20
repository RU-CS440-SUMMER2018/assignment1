# Code that communicates with the c++ plan() function
# in application.cpp

import socket
import sys
import search
import os

# Getting inputs from arguments
writeFd = int(sys.argv[1])
filename = sys.argv[2]
initial_i = int(sys.argv[3])
initial_j = int(sys.argv[4])
goal_i = int(sys.argv[5])
goal_j = int(sys.argv[6])

# Pass arguments to search and get the path vector back
vector = search.search(filename, initial_i, initial_j, goal_i, goal_j)

# Calculate size of vector and let c++ know
numPairs = len(vector)
os.write(writeFd, socket.htonl(numPairs).to_bytes(4, sys.byteorder))

# Send the actual vector
for pair in vector:
    os.write(writeFd, socket.htonl(pair[0]).to_bytes(4, sys.byteorder))
    os.write(writeFd, socket.htonl(pair[1]).to_bytes(4, sys.byteorder))

# Clean up
os.close(writeFd)
