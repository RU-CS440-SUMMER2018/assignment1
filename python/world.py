import astar

class State:
    'Represents a state in this envioronment'
    def __init__(self, i, j):
        self.i = i
        self.j = j
    def __eq__(self, other):
        return (self.i == other.i) and (self.j == other.j)

def manhattanHeuristic(fromNode, toNode):
    '''
    Hueristic that measures the manhattan distence from
    <fromNode> to <toNode> and returns it
    '''
    return abs(toNode.state.i - fromNode.state.i) + abs(toNode.state.j - fromNode.state.j)

def zeroHeuristic(fromNode, toNode):
    'This hueristic always returns 0'
    return 0

def findActions(node, environment):
    
    '''
    Finds all the actions <node> can make
    in <envoronment> and returns them in
    a list
    '''

    actions = []
    i = node.state.i
    j = node.state.j

    for ij in [(i+1,j),(i-1,j),(i,j+1),(i,j-1)]:

        # Save actions that lead you to nodes that are within
        # the maze and are not blocked (a 0)
        try: 
            if (not ij[0] < 0) and (not ij[1] < 0) and environment[ij[0]][ij[1]]:
                neighborNode = astar.Node(State(ij[0], ij[1]), manhattanHeuristic)
                action = astar.Action(1, node, neighborNode)
                neighborNode.setFromAction(action)
                actions.append(action)
        except IndexError:
            continue
    
    return actions
