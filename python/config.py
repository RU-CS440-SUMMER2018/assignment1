# Allocates variables that you can change 
# to easily modify the hueristics of the maze

import world

# Set this variable to a heuristic function builder of your choice.
# A heuristic function builder must follow: (goalNode) => (node) => int.
# Available heuristic functions builders are: world.manhattanHeuristic
# and world.zeroHeuristic.
heuristicFunction = world.manhattanHeuristic

# Set this variable to give the weight to the
# hValue over the gValue when calculating the
# fValue
heuristicWeight = 1
