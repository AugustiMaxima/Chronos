CS452 - K4

Lennox Fei
Xuanji Li

Pointer and Build Guide
This repo can be accessed at the gitlab repo, https://git.uwaterloo.ca/f5fei/chos
To build and install, type make install


Changes
Various changes have been implemented for this assignment.

The new addition, UartServer, is found in /user/library alongside the other suite of syscall level libraries.
It makes use of a worker / server pattern, which combined with an IO optimized ring buffer allows for refined
interface & operations to be exposed to the user with minimal effects latency.

To allow for more fine grained, none blockiong asynchronous operation, the interface AsyncDelay was added to the ClockServer.
It instructs ClockServer to reply to a task waiting for Reply. This allows Workers to Send to an external service, 
and allows the external service to then schedule it's resumption with appropriate delay without blocking itself. This pattern 
is similar in concept to javascript asynchronous timeout. As the implementation replies on a minHeap, the runtime impact is minimal.

In an attempt to reduce the number of notifiers, we introduced AwaitMultiple. This allows a single task to await on multiple 
events, and be unblocked upon one event out of the many it was waiting for. To be conscious of performance implications and minimize runtime,
we use a combination of a AVL tree for look up, taking log n lookup time, and a doubly linked list, allowing the removal of an intermediate element
with O(1) runtime. As a result, AwaitMultiple performs no worse than a Regular AwaitEvent.

Refer to the repo here, and check the latest commit on master to see all the changes.

https://git.uwaterloo.ca/f5fei/chos


Output
We ran out of time and our train control program don't fully work, but the track library is able to switch individual switches and set engine speeds.