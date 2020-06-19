This project aims to implement a simple contingous memory management using C. The memory is implemented by using an array to represent a fixed-size memory pool; each index in the array represents a memory block. The user can typed in the command line a command to perform associated actions. 
The memory pool has these following commands:
- A: allocates a memory chunk for a process. This command comes with 3 different algorithms for allocation: first-fit(F), best-fit(B), and worst-fit(W). Syntax for this command: A <name of the process> <memory> <algorithm>
- F: free up a given process. Syntax for this command : F <name of the process>
- C: compact the memory pool. This command finds all the free blocks, combines them into a large memory chunk, and pushes it to the end of the pool. Syntax for this command : C
- S: display the current status of the memory pool by printing out the pool. Syntax for this command : S
