# Assignment 1

## Protocol

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
