# Assignment 1

## Usage

### Setup

Carefully follow [Rahul Shome's instructions](https://bitbucket.org/rahulshome/cs_440_assignment/wiki/Home)

In this directory, run the following command

    chmod 755 setup.sh && ./setup.sh

### Running a Maze

Modify the heuristic values in `$PRACSYS_PATH/python/config.py` to meet your needs

Now watch our program navigate any maze by running the following command

    roslaunch prx_core assignment_1.launch env:="PATH_TO_MAZE_FILE"

* PATH_TO_MAZE_FILE must point to a file that follows the maze format outlined in this assignment

### Generate a Report

You can start a report server by running

    python3 $PRACSYS_PATH/python/report_server.py numMazes pathsPerMaze outputFile

* `numMazes` is the number of mazes you want to report
* `pathsPerMaze` are teh number of paths per maze you want to report
* `outputFile` is the file where your report will be saved after all paths of each maze complete
* mazes are differentiated by the file of the maze, the heuristic function used, and the hueristic weight used

In this directory, run the following command to automate generating the report outlined in the assignment:

    chmod 755 report.sh && ./report.sh

A new terminal window will open displaying the server. Make sure you keep an eye on the server terminal window because you would need to manually close a maze when the server indicates to move onto the next maze. When all data is recieved, the server will generate a report in `$PRACSYS_PATH/report.txt` and the window will close.

## Implementation

We implemented an A* Search algorithim that finds an optimal path from start to goal. You can see the algorithm implemented in the `aStar()` function in `astar.py`.

`cpp-io.py` calls `search()` in `search.py` passing in appropriate arguments it got from the c++ program. `cpp-io.py` translates and sends `search()`'s return value back to the c++ program. `search()` defines the start and goal nodes, gets the enviornment from the file, calls `aStar()`, and constructs a path list from `aStar()`'s return value.

All files and functions are well commented so you can read them to understand the code in more detail.

### Python-C++ Inter-Process Communication

To impletment our program in python and passing the data to and back from c++ to python, we:

1. Create a pipe in c++
2. Run the python script, `$PRACSYS_PATH/python/cpp-io.py`, as a child process, passing inputs as arguments
3. Write data to the pipe to send back computed output

#### Protocol

These are the command-line arguments passed to the python program from c++

    cpp-io.py write_fd filename initial_i initial_j goal_i goal_j

The python program sends back a stream of bytes that follows the following protocol:

1. First 4 bytes
    * 32-bit network-byte-order integer
    * Represents the number of pairs that will follow
    * Lets call this number N
2. Next 8 bytes
    1. First 4 bytes
        * 32-bit network-byte-order integer
        * Represents first integer in this pair
    2. Second 4 bytes
        * 32-bit network-byte-order integer
        * Represents second integer in this pair
3. Keep going 8 bytes N-1 more times
    * Like #2

### Report

#### Protocol -

4 bytes: length of maze name
maze name
4 bytes: length of herustic name
heuristic name
follow above protocol twice for path and expanded nodes
