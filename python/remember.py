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
    '''
    paths.append(path)

def findPath(startTup, goalTup):

    '''
    Finds a path where using the saved paths
    from startTup to goalTup. Returns the found
    path or None if not found.

    startTup and goalTup are integer tuple pairs
    representing coordinates.
    '''

    for path in paths:
        if startTup in path and goalTup in path:
            startIndex = path.index(startTup)
            goalIndex = path.index(goalTup)
            if goalIndex >= startIndex:
                return path[startIndex:goalIndex+1]
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
    sends the int tuple tup through socket s
    '''
    sendInt(s, tup[0])
    sendInt(s, tup[1])

def getTup(s):
    '''
    returns an int tuple from socket s
    '''
    i = getInt(s)
    j = getInt(s)
    return (i,j)

def handle(s):
    '''
    handles an accepted socket.

    for server.
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
        else:
            sendBool(s, False)
    s.close()

def getPath(startTup, goalTup):
    '''
    for client
    
    gets path from startTup to goalTup
    where both of them are integer pair tuples

    returns path if found, else return None
    '''
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

def sendPath(path):
    '''
    sends an integer tuple list to the server

    this is for the client
    '''
    s = socket.socket()
    s.connect(('localhost', port))
    sendBool(s, True)
    report.sendIntTupList(s, path)
    s.close()

def sendInt(s, num):
    '''
    sends a the integer num through s
    '''
    s.send(socket.htonl(num).to_bytes(4, sys.byteorder))

def getInt(s):
    '''
    return an integer from s
    '''
    return socket.ntohl(int.from_bytes(s.recv(4), sys.byteorder))

def sendBool(s, bool):
    '''
    send boolean bool through s
    '''
    if bool:
        s.send(b's')
    else:
        s.send(b'n')

def getBool(s):
    '''
    get boolean from s
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
