# Contains RPCs used to communicate with
# the report server

import os
import config
import socket
import sys

# Port used for server
port = 8080

# Allocate a list for expanded nodes
expansions = []

def addExpansion(node):
    'Add an expanded node to the list'
    expansions.append((node.state.i, node.state.j))

def report(mazeFilePath, path):

    '''
    Sends mazeFilePath, path, and expansions to the server
    if it is online. path is a list of integer pair tuples
    that represent the path taken in order.
    '''

    # Report only if server is online
    try:
        sock = socket.create_connection(('localhost', 8080))
        sendString(sock, mazeFilePath)
        sendString(sock, config.heuristicFunction.__name__)
        sendIntTupList(sock, path)
        sendIntTupList(sock, expansions)

    # Do nothing if server isn't running
    except ConnectionRefusedError:
        pass

def sendString(sock, string):
    '''
    Send a string through the socket sock with
    the protocol defined in the readme
    '''
    sock.send(socket.htonl(len(string)).to_bytes(4, sys.byteorder))
    sock.send(string.encode())

def sendIntTupList(sock, lst):
    '''
    Send a integer tuple pair list through the socket sock with
    the protocol defined in the readme
    '''
    sock.send(socket.htonl(len(lst)).to_bytes(4, sys.byteorder))
    for (int1, int2) in lst:
        sock.send(socket.htonl(int1).to_bytes(4, sys.byteorder))
        sock.send(socket.htonl(int2).to_bytes(4, sys.byteorder))
