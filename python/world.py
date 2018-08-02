# Contains code specific to solving the maze in this 
# project or world. This code can be midified to meet
# specific needs of another A* Search problem.

import astar
import config

class State:
    'Represents a state in this envioronment'
    def __init__(self, i, j):
        self.i = i
        self.j = j
    def __eq__(self, other):
        return (self.i == other.i) and (self.j == other.j)
    def __str__(self):
        return '(' + str(self.i) + ', ' + str(self.j) + ')'

def manhattanHeuristic(goalNode):
    '''
    Returns a hueristic function ((node) => int)
    that measures the manhattan distence from
    <node> to <goalNode> and returns it
    '''
    def heuristic(node):
        return abs(goalNode.state.i - node.state.i) + abs(goalNode.state.j - node.state.j)
    return heuristic

def zeroHeuristic(goalNode):
    'Returns a hueristic function ((node) => int) that always returns 0'
    def heuristic(node):
        return 0
    return heuristic

def findActions(node, environment, goalNode, heuristicWeight):
    
    '''
    Finds all the actions <node> can make
    in <envoronment> and returns them in
    a list
    '''

    actions = []
    ni = node.state.i
    nj = node.state.j

    for (i, j) in [(ni+1,nj),(ni-1,nj),(ni,nj+1),(ni,nj-1)]:

        # Save actions that lead you to nodes that are within
        # the maze and are not blocked (a 0)
        try: 
            if (not i < 0) and (not j < 0) and environment[i][j]:
                neighborNode = astar.Node(State(i, j), heuristicWeight, config.heuristicFunction(goalNode))
                action = astar.Action(1, node, neighborNode)
                neighborNode.fromAction = action
                actions.append(action)
        except IndexError:
            continue
    
    return actions
