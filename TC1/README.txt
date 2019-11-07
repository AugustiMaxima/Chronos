CS452 - TC1 : 
Lennox Fei

Installation Guide:
Checkout the latest commit of the repo.
Build with make install, which moves the kernel.elf files into ARM/{uw-id}/

For TC1 demonstrations, you will need to be using train #24


Changes:
The primary changes for this assignment is the path finder class. It uses dijkstra's algorithm for finding shortest path given source and destination, and contains parsing that allows the output of dijkstra's algorithm to be converted to track data pertaining to events (i.e. needing to reverse or switch tracks)
While currently, reverse is not supported, but this can be easily added using the reverse pointer and treating its distance as either a 0 edge or an low edge node.
On each iteration, only the minimal weight edge is selected.

