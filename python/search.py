# This is the search function you guys need to populate.
#
# Given filename of the enviornment, the initial coordinates
# and the goal coordinates.
# 
# You must return a list of tuples. Each tuple must be a pair
# or integers.
#
# Currently This has basically the same simple algorithm that
# the original c++ code has as a demonstration.
# 
# Feel free to add whatever functions or variables you like in this file.
def search(filename, initial_i, initial_j, goal_i, goal_j):

    retList = []
    retList.append((initial_i,initial_j))

    i = initial_i
    while i < goal_i:
        i += 1
        retList.append((i, initial_j))
    i = initial_i
    while i > goal_i:
        i -= 1
        retList.append((i, initial_j))
    j = initial_j
    while j < goal_j:
        j += 1
        retList.append((goal_i, j))
    j = initial_j
    while j > goal_j:
        j -= 1
        retList.append((goal_i, j))

    return retList

# Returns a 2D list[i][j] representing
# the maze in filename
def readMaze(filename):

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
    for i in range(cols):
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
