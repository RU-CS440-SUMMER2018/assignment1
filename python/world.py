import astar

class State:
    def __init__(self, i, j):
        self.i = i
        self.j = j
    def __eq__(self, other):
        return (self.i == other.i) and (self.j == other.j)

def manhattanHeuristic(fromNode, toNode):
    return abs(toNode.state.i - fromNode.state.i) + abs(toNode.state.j - fromNode.state.j)

def zeroHeuristic(fromNode, toNode):
    return 0

def findActions(node, environment):

    actions = []
    ni = node.state.i
    nj = node.state.j

    for i in [ni+1, ni-1]:
        for j in [nj+1, nj-1]:
            if environment[i][j]:
                neighborNode = astar.Node(State(i, j), manhattanHeuristic)
                action = astar.Action(1, node, neighborNode)
                neighborNode.setFromAction(action)
                actions.append(action)
    
    return actions




