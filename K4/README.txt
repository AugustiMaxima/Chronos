CS452 - K4V2

Lennox Fei

Pointer and Build Guide
This repo can be accessed at the gitlab repo, https://git.uwaterloo.ca/f5fei/chos
Check out the latest commit. (Before this file is checked in, the latest hash is 9d510c2c9f78716a307162602682e37926759572). Put the latest from master.
To build and install, type make install.
You should be able to run this via load ARM/{uw id}/kernel.elf

It will prompt you for the track A/B. Enter A, a, B, b depending on which track you are using. Do not precede with any other character or it will quit.

Changes
Various changes have been implemented for this assignment.

The new addition, UartServer, is found in /user/library alongside the other suite of syscall level libraries.
It makes use of a worker / server pattern, which combined with an IO optimized ring buffer allows for refined
interface & operations to be exposed to the user with minimal effects on latency.

To unblock the server while it is waiting for workers to unblock tasks that require resource contention, we have made use of a courier pattern for
coordinating between the server and the worker, as well as an asynchronous task queue to allow blocking requests to be queued up and served, rather than
the more costly fail/retry model that would otherwise have been used.

To allow for more fine grained, none blockiong asynchronous operation, the interface AsyncDelay was added to the ClockServer.
It instructs ClockServer to reply to a task waiting for Reply. This allows Workers to Send to an external service, 
and allows the external service to then schedule it's resumption with appropriate delay without blocking itself. This pattern 
is similar in concept to javascript asynchronous timeout. As the implementation replies on a minHeap, the runtime impact is minimal.

In an attempt to reduce the number of notifiers, we introduced AwaitMultiple. This allows a single task to await on multiple 
events, and be unblocked upon one event out of the many it was waiting for. To be conscious of performance implications and minimize runtime,
we use a combination of a AVL tree for look up, taking (log n) lookup time, and a doubly linked list, allowing the removal of an intermediate element
with O(1) runtime. AwaitMultiple's runtime is bounded by O(k + log n), where k is the number of different events being awaited by the task we need to remove.
As k is bounded by 5, this can be considered a constant and do not determiment runtime significantly.

Finally, the implmentation of the UI remains a high point of this kernel. UI gets a dedicated thread, and keeps a RenderUpdate pointer that allows for the coordination between the ui thread with other component.
Conceptually, this is closer to a thread model within the process that shares similar memory space, but even if implemented in Processes instead, we can still allow for shared memory in a "real" kernel.
As other processes can only write True while the TUI writes False, this is unlikely to result in race condition. Utiliziing the component update model, and using special operations on UI server and transmitBuffer
to gather input, this allows for an extremely decoupled ui that is responsive, non blocking and minimizes cpu time, leading to increasing idle percentage from 70 to 99 percent on average, while still producing far
lower input latency and improving the responsiveness of the ui elements.

Output:
The terminal will respond to commands and show what you are typing as a command on screen. It reacts to:
tr train_number speed
rv train_number
sw branch_number direction (C/S)
q

Since the output is mostly ui, here is a rough rundown of the notes
Sensor inputs are displayed in a round robin format, with the arrow indicating the last activated sensor.
Branch states are arrayed on line 6, and owning to the space limit, the switches are displayed in order from branch 1 to 18 and then 153 - 156.
Idle time is displayed on the upper left corner, while the time is displayed on the upper right corner.
Note about input: The default input buffer is 10 characters long, so entering commands longer than 9 characters without enter will lead to it being ignored.