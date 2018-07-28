# Assignment 1

## Usage

1. Carefully follow [Rahul Shome's instructions](https://bitbucket.org/rahulshome/cs_440_assignment/wiki/Home)
2. In this directory, run the following command
    ```bash
    chmod 755 setup.sh && ./setup.sh
    ```
3. Now watch our program navigate any maze by running the following command
    ```bash
    roslaunch prx_core assignment_1.launch env:="PATH_TO_MAZE_FILE"
    ```
    * PATH_TO_MAZE_FILE must point to a file that follows the maze format outlined in this assignment

## Implementation

### Python-C++ Inter-Process Communication

#### Protocol

cpp-io.py port filename initial_i initial_j goal_i goal_j

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
