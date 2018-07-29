import world
import astar

def search(filename, initial_i, initial_j, goal_i, goal_j):

    '''    
    <filename> is the file that contains the enviornment.
    the rest of the arguments are the initial and goal coordinates.
    
    Returns a list of tuples. Each tuple is a pair of integers.
    '''

    startState = world.State(initial_i, initial_j)
    goalState = world.State(goal_i, goal_j)

    startNode = astar.Node(startState, world.manhattanHeuristic)
    goalNode = astar.Node(goalState, world.manhattanHeuristic)
    environment = readMaze(filename)

    foundGoal = astar.aStar(startNode, goalNode, environment)
    while foundGoal:
        fromAction = foundGoal.fromAction
        i = foundGoal.state.i
        j = foundGoal.state.j
        print('(' + str(i) + ', ' + str(j) + ')')
        if fromAction:
            foundGoal = fromAction.fromNode
        else:
            foundGoal = None

def readMaze(filename):
    '''
    Returns a 2D list[i][j] representing
    the maze in filename
    '''

    # Read file
    file = open(filename, 'r')
    lines = file.readlines()
    file.close()
    
    # Get rows and columns
    rows = int(lines[0])
    cols = int(lines[1])
    del lines[0]
    del lines[0]

    # Initialize the return list
    retList = []
    for _ in range(cols):
        retList.append([])
    
    # Save integers to return list
    col = 0
    while col < cols:
        row = 0
        while row < rows:
            retList[col].append(int(lines[col][row*2]))
            row += 1
        col += 1

    return retList