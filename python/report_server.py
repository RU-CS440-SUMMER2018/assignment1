import os
import config
import socket
import threading
import report
import sys

sock = socket.socket()
sock.bind(('localhost', report.port))
sock.listen(10)

lock = threading.Lock()
mazeDict = {}
numberOfMazes = 8
pathsPerMaze = 10

def exitIfDone():

    lock.acquire()
    
    done = False
    if len(mazeDict) == numberOfMazes:
        done = True
        for key in mazeDict:
            if len(mazeDict[key][0] < pathsPerMaze):
                done = False
                break

    if done:

        for key in mazeDict:
            numInPath = 0
            numExpanded = 0
            for path in mazeDict[key][0]:
                numInPath += len(path)
            for expanded in mazeDict[key][1]:
                numExpanded += len(expanded)
            print(key)
            print('Number of nodes in 10 paths: ' + str(numInPath))
            print('Number of expanded nodes in 10 paths: ' + str(numExpanded))

        sock.close()
        exit()
        
    else:
        for key in mazeDict:
            numPaths = len(mazeDict[key][0])
            print(key)
            print('Paths completed: ' + str(numPaths))

    lock.release()

def savePath(mazeName, heuristicName, heuristicWeight, pathList, expandedNodesList):

    key = getMazeKey(mazeName, heuristicName, heuristicWeight)

    try:
        mazeDict[key]
    except KeyError:
        mazeDict[key] = ([], [])

    mazeDict[key][0].append(pathList)
    mazeDict[key][1].append(expandedNodesList)


def getMazeKey(mazeName, heuristicName, heuristicWeight):
    return 'Maze: ' + mazeName + '\nHeuristic: ' \
        + heuristicName + '\nWeight: ' + str(heuristicWeight)

def getString(sock):
    'Get a string from <sock> as defined by our protocol'
    mazeNameLen = socket.ntohl(int.from_bytes(sock.recv(4), sys.byteorder))
    mazeName = sock.recv(mazeNameLen).decode()
    return mazeName

def getIntTupList(sock):
    'Get a list of int pairs from <sock> as defined by our protocol'
    numTups = socket.ntohl(int.from_bytes(sock.recv(4), sys.byteorder))
    lst = []
    for _ in range(numTups):
        int1 = socket.ntohl(int.from_bytes(sock.recv(4), sys.byteorder))
        int2 = socket.ntohl(int.from_bytes(sock.recv(4), sys.byteorder))
        lst.append((int1, int2))
    return lst

def handle(s):
    lock.acquire()

    mazeName = getString(s)
    heuristicName = getString(s)
    path = getIntTupList(s)
    expandedNodes = getIntTupList(s)

    savePath(mazeName, heuristicName, config.heuristicWeight, path, expandedNodes)
    
    s.close()
    lock.release()

while True:
    exitIfDone()
    newSock = sock.accept()[0]
    thread = threading.Thread(target=handle, args=(newSock,))
    thread.start()
