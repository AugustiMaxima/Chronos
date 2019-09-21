/*
    Interface for all the kernel end sys call handlers

    Formal description of how syscalls are handled, hopefully this will also help with interrupts in the future

    User stack state goes like this:

    Arg 0           <- SP currently points here
    ...             
    Arg N
    CPSR
    R0-15           <- Stored SP points here

    Individual handlers are dispatched by the sys_handler, and will know the size of N since function signatures are fixed.
    With enough effort, you can implement variadic arguments on the sys calls, but it would be quite difficult to wind and unwind

    Since individual handler functions knows the number of arguments, we can safely unwind back to the CPSR point
    At this point, the handlers functions are responsible for updating the stackEntry field in the current task to the content of the SP
    This order is important as stackEntry is used by the exitKernel to use as the beginning point of the function
*/

//The generic handler
//Redirects to the approrpiate handler based on the interrupt code
void sys_handler(int code);

//Eventually
//void interrupt_handler();

void getPid();

//no arguments because we will be manually retrieving it from the user stack
void createTask();