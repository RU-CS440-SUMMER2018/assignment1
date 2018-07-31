import heapq
import world
import report

class Node:
    
    '''
    Instances represents a node.

    <state> is the state at this node.

    <heuristicFucntion> is a function used to measure
    the heristics. (fromNode, toNode) => int. Must return
    the heuristic value between <fromNode> and <toNode>.

    <fromAction> is the action taken to get from previous node
    in the path to this node.
    '''

    def __init__(self, state, heuristicFucntion, fromAction=None):
        self.state = state
        self.fromAction = fromAction
        self.heuristicFucntion = heuristicFucntion

    def setFromAction(self, action):
        'Set the action of this node'
        self.fromAction = action

    def hValue(self, toNode):
        'Get the heuristic value between this node and <toNode>'
        return self.heuristicFucntion(self, toNode)

    def gValue(self):
        'Get the value from the initial node to this node'
        if self.fromAction:
            return self.fromAction.cost + self.fromAction.fromNode.gValue()
        else:
            return 0

    def fValue(self, toNode, weight):
        '''
        Get the value of initial node to this node plus the
        heuristic between this node and <toNode>. The higher
        the value of weight is, less priority will be given
        to the heuristic
        '''
        return self.gValue() + (weight * self.hValue(toNode))

    def __eq__(self, other):
        return self.state == other.state
    
    def __gt__(self, other):
        return self.gValue() > other.gValue()

class Action:
    '''
    Represents an action taken from <fromNode> to
    <toNode> with a cost of <cost>
    '''
    def __init__(self, cost, fromNode, toNode):
        self.cost = cost
        self.fromNode = fromNode
        self.toNode = toNode

class NodeQueue:
    '''
    Priority queue based on nodes with the least fValue
    to <goalNode>. <heuristicWeight> gives weight to
    hValue over gValue in fValue.
    '''
    def __init__(self, goalNode, heuristicWeight):
        self.lst = []
        self.goalNode = goalNode
        self.heuristicWeight = heuristicWeight
    def push(self, node):
        heapq.heappush(self.lst, (node.fValue(self.goalNode, self.heuristicWeight), node))
    def pop(self):
        return heapq.heappop(self.lst)[1]
    def contains(self, node):
        for tup in self.lst:
            if node == tup[1]:
                return True
        return False
    def find(self, node):
        'Returns a node that is equivalent to <node>'
        for tup in self.lst:
            if node == tup[1]:
                return tup[1]
        return None
    def update(self, node):
        '''
        Replaces the node in the queue that is equivalent
        to <node> with <node>
        '''
        for i in range((self.lst)):
            if node == self.lst[i][1]:
                del self.lst[i]
                self.push(node)
                break
        return None
    def isEmpty(self):
        return len(self.lst) == 0

def aStar(startNode, goalNode, environment, heuristicWeight):

    '''
    Performs A* search from <startNode> to <goalNode>
    in <environment>. Returns a node that is equivalent
    to <goalNode> if the path is found, else returns None.
    <heuristicWeight> gives a weight to the hValue over
    the gValue in fValue.
    '''

    # Initialize lists
    openList = NodeQueue(goalNode, heuristicWeight)
    openList.push(startNode)
    closedList = []

    # Perform as long as there are nodes in openList
    while not openList.isEmpty():

        # Mark best known node as viited
        # and return if it is the goal
        currentNode = openList.pop()
        if currentNode == goalNode:
            return currentNode
        closedList.append(currentNode)

        # Add node coordinates to be reported
        report.addExpansion(currentNode)

        # Go through all actions of the node
        for action in world.findActions(currentNode, environment):

            # Look at the neighbors that haven't been visited
            if closedList.count(action.toNode) == 0:
                neighborNode = action.toNode

                # Add the neighbor to the openList if not already
                if not openList.contains(neighborNode):
                    openList.push(neighborNode)
                
                # If the neighbor is in the openList, check and update if the
                # new values are better than the old ones
                else:
                    oldNeighborNode = openList.find(neighborNode)
                    if neighborNode.fValue(goalNode, heuristicWeight) < oldNeighborNode.fValue(goalNode, heuristicWeight):
                        openList.update(neighborNode)

    # Return if goal not found
    return None
