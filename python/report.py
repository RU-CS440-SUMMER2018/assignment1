import os
import config
import socket
import sys

port = 8080
expansions = []

# print('test')

def addExpansion(node):
    expansions.append((node.state.i, node.state.j))

def getCoordinateString(node):
    i = node[0]
    j = node[1]
    return getIjString(i, j)

def getIjString(i, j):
    return '(' + str(i) + ', ' + str(j) + ')'

def report(mazeFilePath, path):
    
    try:
        sock = socket.create_connection(('localhost', 8080))
        sendString(sock, mazeFilePath)
        sendString(sock, config.heuristicFunction.__name__)
        sendIntTupList(sock, path)
        sendIntTupList(sock, expansions)
    except ConnectionRefusedError:
        pass

def sendString(sock, string):
    sock.send(socket.htonl(len(string)).to_bytes(4, sys.byteorder))
    sock.send(string.encode())

def sendIntTupList(sock, lst):
    sock.send(socket.htonl(len(lst)).to_bytes(4, sys.byteorder))
    for (int1, int2) in lst:
        sock.send(socket.htonl(int1).to_bytes(4, sys.byteorder))
        sock.send(socket.htonl(int2).to_bytes(4, sys.byteorder))
