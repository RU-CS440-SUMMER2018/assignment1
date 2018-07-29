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
    i = node.state.i
    j = node.state.j

    for ij in [(i+1,j),(i-1,j),(i,j+1),(i,j-1)]:
        try: 
            if (not ij[0] < 0) and (not ij[1] < 0) and environment[ij[0]][ij[1]]:
                neighborNode = astar.Node(State(ij[0], ij[1]), manhattanHeuristic)
                action = astar.Action(1, node, neighborNode)
                neighborNode.setFromAction(action)
                actions.append(action)
        except IndexError:
            continue
    
    return actions
