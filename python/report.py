import os
import config

expansions = []

def addExpansion(node):
    expansions.append((node.state.i, node.state.j))

def getCoordinateString(node):
    i = node[0]
    j = node[1]
    return getIjString(i, j)

def getIjString(i, j):
    return '(' + str(i) + ', ' + str(j) + ')'

def report(mazeFilePath, path):

    mazeFileName = mazeFilePath.split('/')[-1]
    reportFilePath = os.environ['PRACSYS_PATH'] \
        + '/' + 'report_' + config.heuristicFunction.__name__ \
        + '_' + 'weight' + str(config.heuristicWeight) + '_' + mazeFileName
    file = open(reportFilePath, 'a')

    startTuple = path[0]
    goalTuple = path[-1]

    file.write('Start: ' + getCoordinateString(startTuple) + '\n')
    file.write('Goal: ' + getCoordinateString(goalTuple) + '\n')

    file.write('Path')
    for coordinate in path:
        file.write(' -> ' + getCoordinateString(coordinate))
    file.write('\n\nNumber of steps: ' + str(len(path)))

    file.write('\n\nExpanded nodes')
    for coordinate in expansions:
        file.write(' -> ' + getCoordinateString(coordinate))
    file.write('\n\nNumber of expanded nodes: ' + str(len(expansions)))

    file.write('\n\n================================================\n\n')
    file.close()
    