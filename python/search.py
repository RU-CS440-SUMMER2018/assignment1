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
