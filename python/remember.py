# This file contains code used for storing
# visited paths and using this data to not
# recompute paths. Has server and client code.

import threading
import socket
import sys
import report_server
import report

port = 8081

def savePath(path):

    '''
    Saves path to memory.

    path is list of integer pair tuples
    that represetns a path.

    For server.
    '''

    # Save the path
    paths.append(path)

    # Let user know path is saved
    print('Recorded paths:')
    for i in range(len(paths)):
        report_server.printTupList(paths[i], 'Path ' + str(i+1))
    print('\n')

def findPath(startTup, goalTup):

    '''
    Finds a path where using the saved paths
    from startTup to goalTup. Returns the found
    path or None if not found.

    startTup and goalTup are integer tuple pairs
    representing coordinates.

    For server
    '''

    # Loops through paths
    for path in paths:

        # If both the goal and start are in a path
        # this is the one we need
        if startTup in path and goalTup in path:

            # Get indexs of tuples
            startIndex = path.index(startTup)
            goalIndex = path.index(goalTup)

            # If goal index is larger, return the split list
            if goalIndex >= startIndex:
                return path[startIndex:goalIndex+1]

            # If start is larger, return the reversed split list
            else:
                reversedPath = path[goalIndex:startIndex+1]
                reversedPath.reverse()
                return reversedPath

    return None

def sendStartGoal(s, start, goal):
    '''
    Sends the int tupes start and goal
    through socket s
    '''
    sendTup(s, start)
    sendTup(s, goal)

def sendTup(s, tup):
    '''
    Sends the int tuple tup through socket s
    '''
    sendInt(s, tup[0])
    sendInt(s, tup[1])

def getTup(s):
    '''
    Returns an int tuple from socket s
    '''
    i = getInt(s)
    j = getInt(s)
    return (i,j)

def handle(s):
    '''
    Handles an accepted socket. For server.
    '''
    if getBool(s):
        lst = report_server.getIntTupList(s)
        savePath(lst)
    else:
        start = getTup(s)
        goal = getTup(s)
        path = findPath(start, goal)
        if path:
            sendBool(s, True)
            report.sendIntTupList(s, path)
            report_server.printTupList(path, 'Path found and sent')
            print('\n')
        else:
            sendBool(s, False)
    s.close()

def getPath(startTup, goalTup):

    '''
    For client
    
    Gets path from startTup to goalTup
    where both of them are integer pair tuples

    Returns path if found, else return None.
    Also returns None if server offline
    '''

    try: 
        s = socket.socket()
        s.connect(('localhost', port))
        sendBool(s, False)
        sendStartGoal(s, startTup, goalTup)
        if getBool(s):
            path = report_server.getIntTupList(s)
            s.close()
            return path
        else:
            s.close()
            return None

    except ConnectionRefusedError:
        return None

def sendPath(path):
    '''
    Sends an integer tuple list to the server

    This is for the client
    '''
    try:
        s = socket.socket()
        s.connect(('localhost', port))
        sendBool(s, True)
        report.sendIntTupList(s, path)
        s.close()
    except ConnectionRefusedError:
        pass

def sendInt(s, num):
    '''
    Sends a the integer num through s
    '''
    s.send(socket.htonl(num).to_bytes(4, sys.byteorder))

def getInt(s):
    '''
    Return an integer from s
    '''
    return socket.ntohl(int.from_bytes(s.recv(4), sys.byteorder))

def sendBool(s, bool):
    '''
    Send boolean bool through s
    '''
    if bool:
        s.send(b's')
    else:
        s.send(b'n')

def getBool(s):
    '''
    Get boolean from s
    '''
    data = s.recv(1)
    if data == b's':
        return True
    return False

# Start the server
if __name__ == '__main__':

    # Allocate memory for paths
    paths = []

    # Create server socket
    sock = socket.socket()
    sock.bind(('localhost', port))
    sock.listen(10)
    lock = threading.Lock()

    # Run server aslong as more data is needed
    while True:
        newSock = sock.accept()[0]
        thread = threading.Thread(target=handle, args=(newSock,))
        thread.start()
