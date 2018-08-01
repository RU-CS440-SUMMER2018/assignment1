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
numberOfMazes = int(sys.argv[1])
pathsPerMaze = int(sys.argv[2])
reportFile = sys.argv[3]

def exitIfDone():
    lock.acquire() 
    done = False
    if len(mazeDict) == numberOfMazes:
        done = True
        for key in mazeDict:
            if len(mazeDict[key][0]) < pathsPerMaze:
                done = False
                break
    if done:
        with open(reportFile, 'w') as file:
            for key in mazeDict:
                numInPath = 0
                numExpanded = 0
                for path in mazeDict[key][0]:
                    numInPath += len(path)
                for expanded in mazeDict[key][1]:
                    numExpanded += len(expanded)
                file.write(key)
                file.write('\nNumber of nodes in ' + str(pathsPerMaze) + ' paths: ' + str(numInPath))
                file.write('\nNumber of expanded nodes in ' + str(pathsPerMaze) + ' paths: ' + str(numExpanded) + '\n\n')
                print('The report has been saved as: ' + reportFile)
        sock.close()
        exit()
    lock.release()

def printTupList(lst, label):
    for tup in lst:
        label += ' -> ' + str(tup)
    print(label + '\n')

def savePath(mazeName, heuristicName, pathList, expandedNodesList):
    key = getMazeKey(mazeName, heuristicName)
    try:
        mazeDict[key]
    except KeyError:
        mazeDict[key] = ([], [])
    pathsCompleted = len(mazeDict[key][0])
    if pathsCompleted < pathsPerMaze:
        mazeDict[key][0].append(pathList)
        mazeDict[key][1].append(expandedNodesList)
        pathsCompleted = len(mazeDict[key][0])
        print(key + '\n')
        printTupList(pathList, 'Path (' + str(len(pathList)) + ' nodes)')
        printTupList(expandedNodesList, 'Expanded nodes (' + str(len(expandedNodesList)) + ' nodes)')
        print(str(pathsCompleted) + ' paths completed')
        print(str(pathsPerMaze - pathsCompleted) + ' paths left')
        print('\n================================================\n')
    else:
        print(str(pathsCompleted) + ' paths already completed, move onto the next maze')
        numMazeCompleted = 0
        for key in mazeDict:
            if len(mazeDict[key][0]) == pathsPerMaze:
                numMazeCompleted += 1
        print(str(numMazeCompleted) + ' mazes completed')
        print(str(numberOfMazes - numMazeCompleted) + ' mazes left\n')

def getMazeKey(mazeName, heuristicName):
    return 'Maze: ' + mazeName + '\nHeuristic: ' \
        + heuristicName + '\nWeight: ' + str(config.heuristicWeight)

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
    savePath(mazeName, heuristicName, path, expandedNodes)
    s.close()
    lock.release()

while True:
    exitIfDone()
    newSock = sock.accept()[0]
    thread = threading.Thread(target=handle, args=(newSock,))
    thread.start()
