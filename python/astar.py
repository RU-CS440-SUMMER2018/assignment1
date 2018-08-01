import heapq
import world
import report

class Node:
    
    '''
    Instances represents a node.

    <state> is the state at this node.

    <heuristicWeight> gives weight to the hValue over
    gValue when computing the fValue.

    <heuristicFucntion> is a function used to measure
    the heristics. (node) => int. Must return
    the heuristic value of <node>.

    <fromAction> is the action taken to get from previous node
    in the path to this node.

    Comparing 2 nodes compares their fValues.
    '''

    def __init__(self, state, heuristicWeight, heuristicFucntion=None, fromAction=None):
        self.state = state
        self.fromAction = fromAction
        self.heuristicFucntion = heuristicFucntion
        self.heuristicWeight = heuristicWeight

    def hValue(self):
        'Get the heuristic value for this node'
        return self.heuristicFucntion(self)

    def gValue(self):
        'Get the cost from the initial node to this node'
        if self.fromAction:
            return self.fromAction.cost + self.fromAction.fromNode.gValue()
        else:
            return 0

    def fValue(self):
        '''
        Get the cost of initial node to this node plus the
        heuristic value of this node
        '''
        return self.gValue() + (self.heuristicWeight * self.hValue())

    def __eq__(self, other):
        'Compares fValue of nodes'
        return self.fValue() == other.fValue()
    
    def __gt__(self, other):
        'Compares fValue of nodes'
        return self.fValue() > other.fValue()

    def __str__(self):
        return self.state.__str__()

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
    def __init__(self):
        self.lst = []
    def push(self, node):
        heapq.heappush(self.lst, node)
    def pop(self):
        return heapq.heappop(self.lst)
    def contains(self, node):
        return not ([node.state for node in self.lst].count(node.state) == 0)
    def find(self, node):
        '''
        Returns a node that is equivalent to <node>.
        Throws ValueError if <node> not found.
        '''
        return self.lst[[node.state for node in self.lst].index(node.state)]
    def update(self, node):
        '''
        Replaces the node in the queue that is equivalent
        to <node> with <node>
        '''
        del self.lst[[node.state for node in self.lst].index(node.state)]
        self.push(node)
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
    openList = NodeQueue()
    openList.push(startNode)
    closedList = []

    # Perform as long as there are nodes in openList
    while not openList.isEmpty():

        # Mark best known node as viited
        # and return if it is the goal
        currentNode = openList.pop()
        if currentNode.state == goalNode.state:
            return currentNode
        closedList.append(currentNode)
        
        # Report the node as being expanded
        report.addExpansion(currentNode)

        # Go through all actions of the node
        for action in world.findActions(currentNode, environment, goalNode, heuristicWeight):

            # Look at the neighbors that haven't been visited
            if [node.state for node in closedList].count(action.toNode.state) == 0:
                neighborNode = action.toNode

                # If the neighbor is in the openList, check and update if the
                # new values are better than the old ones
                try:
                    oldNeighborNode = openList.find(neighborNode)
                    if neighborNode < oldNeighborNode:
                        openList.update(neighborNode)

                # Add the neighbor to the openList if not already
                except ValueError:
                    openList.push(neighborNode)
                    
    # Return if goal not found
    return None
