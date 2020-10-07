
- [Lecture 1 Introduction](#lecture-1-introduction)
  - [1 Process](#1-process)
  - [2 Protection and resource management](#2-protection-and-resource-management)
  - [3 Main techniques for virtualization](#3-main-techniques-for-virtualization)
  - [4 Exercise for lecture 1](#4-exercise-for-lecture-1)
- [Lecture 2 Memory Management free space and dynamic allocation](#lecture-2-memory-management-free-space-and-dynamic-allocation)
  - [1 Fragmentation](#1-fragmentation)
  - [2 How to implement a memory allocator](#2-how-to-implement-a-memory-allocator)
  - [3 Beyond simple free lists](#3-beyond-simple-free-lists)
  - [4 Garbage collection](#4-garbage-collection)
- [Lecture 3 The memory hierarchy](#lecture-3-the-memory-hierarchy)
  - [1 Cache](#1-cache)
  - [2 Memory Hierarchies](#2-memory-hierarchies)
    - [1 why](#1-why)
    - [2 examples](#2-examples)
- [Lecture 4 Threads](#lecture-4-threads)
  - [1 why threads](#1-why-threads)
  - [2 Preemptive and cooperative threads](#2-preemptive-and-cooperative-threads)
  - [3 Kernel threads and user threads](#3-kernel-threads-and-user-threads)
- [Lecture 5 Virtual Memory](#lecture-5-virtual-memory)
  - [Issues in sharing physical memory](#issues-in-sharing-physical-memory)
  - [Goal of virtual memory](#goal-of-virtual-memory)
  - [Advantage](#advantage)
  - [Implement ways](#implement-ways)
  - [Virtual memory definitions](#virtual-memory-definitions)
  - [Segmentation](#segmentation)
  - [Page](#page)
# Lecture 1 Introduction

## 1 Process

A process is an abstraction corresponding to a running instance of a program.
Its main role consists in virtualizing the CPU.
A process mainly consists in an execution context, a memory space, a logical state.

## 2 Protection and resource management

Prevent bad processes from impacting the OS or other processes.

**Preemption**: give a resource to a process and take it away if needed.Protection mechanism to prevent a process from monopolizing the CPU

**Interposition**: place OS between application and resources.(tracks resources, access)

**CPU execution modes**: OS kernel code runs in privileged mode. Application code runs in unprivileged mode.

**System calls**:applications invoke kernel services through the system call mechanism.Perform things that an application is not allowed to do in unprivileged mode.

**CPU scheduling**:in charge of deciding which process should run on the CPU.Periodically(for each timer interrupt) and Punctually(in reaction to syscalls and interrupts).

**Context Switch**
1 make decision on the process p2 that should obtain the CPU
2 save execution context of "outgoing" process p1
3 inject/restore the execution context of p2 on the CPU
between two execution contexts and has *a non-negligible cost*

**Mode Switch**:==between user and kernel mode in the same execution context==

**Different system contexts**
User-level process context
Kernel process context: running kernel code on behalf of a particular process(e.g. system call)
Kernel code not associated with a process(e.g. timer\device interrupt handler)

**Memory virtualization and protection**
OS must protect the memory space of a process from the actions of other processes

* **Address space** all memory locations that a program can name
* **Virtual space** an address in a process address space
* **Physical space** an address in real memory
* **Address translation**map virtual address to physical address

## 3 Main techniques for virtualization

* **Multiplexing** Exposes a resource among multiple virtual entities
* **Aggregation** Takes multiple resources and makes them appear as a single abstraction.
* **Emulation**Expose (using a level of indirection in software ) a virtual resource that is not provided by the underlying machine.  

## 4 Exercise for lecture 1

1 ![Exercise 1](https://raw.githubusercontent.com/Eizz0/cloudimg/master/data/1.jpg)

The typical duration of 10ms for the time quantum of an operating system scheduler is motivated by the fact that, on most machines, the hardware cannot generate timer interrupts with a higher frequency. ***It' false because it can generate timer interrupts with a higher frequency choose 10ms just beacause it's a compromise.***

2 ![Exercise 2](https://raw.githubusercontent.com/Eizz0/cloudimg/master/data/3.jpg)

c) blocking hardware interrupts is allowed***i.e. handling a timer interrupt that also manipulates the list of processes***  

d) triggering the execution of the process scheduler is allowed ***i.e. exit(0), sleep()***

3 ![Exercise 3](https://raw.githubusercontent.com/Eizz0/cloudimg/master/data/2.jpg)

The first design is fragile and will waste of memory.  
The second one is better in safety and maintainability.Easy to change and can be adapted to new CPU.  

4 Give an example of physical resource that is both time shared and space shared by an operating system.  

Disk Memory CPU  

5 process and child process  
  
```[c]

int(i=0;i<2;i++){
    if(pid == 0){
        printf("%d ", i);
        exit(0);
    }
}
printf("P")

```

there are 3 process 1 parent and 2 children.
***take care of the exit(0)*** and the output is random like P01 0P1 01P.

# Lecture 2 Memory Management free space and dynamic allocation 

## 1 Fragmentation  

- External Fragmentation  
when there is enough aggregate heap memory, but no single free block is large enough.**1 size heterogeneity; 2 isolated deaths; 3 Time-varying behavior(free blocks requests larger ones)**

- Internal Fragmentation  
For a given block, inter fragmentation occurs if payload is smaller than block size. **1 overhead of maintaining heap data structures 2 padding for alignment purposes 3 explicit policy decisions**  

## 2 How to implement a memory allocator  

- Free block organization : use "free list" -- linked list of descriptors of free blocks. ***Bookkeeping(store size of block + pointers to next and previous elements in free list) + memory alignment***
  
- Placement : the strategies have a major impact on external fragmentation.  
     1. best fit : minimize fragmentation by allocating space from block that ***leaves smallest fragment***sawdust everywhere (as a result of small remainder)  
        a list of free blocks each has a header holding block size and pointer to next
     2. first fit : ***pick the first block that fits*** LIFO \ FIFO \ Address sort(similar to best fit)  problem: sawdust(sorting the list forces a large request to skip many small blocks) at beginning of the list  
     3. 20 -> 15 ------ 10 , 20  best fit wins ; 8 , 12 , 12 first fit wins

- Splitting :
  1. keep the remainder within the chosen block ***simple and fast but introduces more internal fragmentation***  
  2. split the chosen block in two and insert the remainder block in the free list  

- Coalescing : helps avoiding "false external fragmentation" ; Immediate or Deferred.

<mark><b>Impossible to solve fragmentation</mark></b>

> there exists streams of allocation and de-allocation requests that defeat the allocator and force it into severe fragmentation.

## 3 Beyond simple free lists  

- Fighting fragmentation by Exploiting ordering and size dependencies Segregated List (Seglist) Allocators (Each class of blocks has its own free list)  

- Slab allocation : optimize for the case : **task_struct** needs contiguous physical memory, a slab is multiple pages of contiguous physical memory. ***speed and no internal fragmentation***

- Buddy allocation : a special form of segregated allocator. Binary buddies : rounds up all allocation to powers of 2 ***fast but may result in heavy internal fragmentation***  

## 4 Garbage collection

1. goal : automatica reclamation of heap-allocated storage ; the application never has to free(avoid memory bugs: double free bugs, 8dangling pointers, memory leaks)

2. Main principle : ***what is garbage a memory block cannot be reached anymore by the application***  

3. assumptions : view memory as a directed graph(each block is a node in the graph; each pointer is an edge in the graph;Locations not in the heap that contain into the heap called root nodes ***e.g.,registers,global variables,locations on the stack***)

4. algorithms :
    1. Tracing collectiors (triggered when heap runs out of free space)

    2. Reference counting : use ref count that keeps tracks of the current number of pointers to it


# Lecture 3 The memory hierarchy

## 1 Cache
 
Computer memory with short access time used for the storage of frequently or recently used instructions or data.


## 2 Memory Hierarchies

### 1 why
Faster storage cost more per byte and have lower capacity, gaps between memory technology speeds are widening , it's ***an approach for organizing memory and storage systems***

### 2 examples




# Lecture 4 Threads

***A thread is a schedulable execution context***

## 1 why threads

concurrent : one cpu , parallel : different cpus

1. most popular abstraction for concurreny: all threads in a process share memory and file descriptors, lower resource comsumption compare to process

2. allows a process to use multiple cpus

3. allows a program to overlap I/O and computation 

## 2 Preemptive and cooperative threads

1. preemptive threads:  a thread can be preempted at any time in order to allocate the CPU to another context, thread(the same process), process; ***multiple threads within the same process can run in parallel on multiple CPUs.***

2. cooperative threads : at most a single thread (within a given process) is allowed to run at a given point in time; a thread switch(in process) can only happen when ***relinquishes the CPU(yield()) or issues a blocking syscall/terminates.***

3. discussion  

   - preemptive threads cause more "race conditions"  
   - cooperative threads cannot take advantage of multiple CPUs
   - cooperative threads may let a "misbehaving"

## 3 Kernel threads and user threads

1. kernel threads **kernel-managed threads** : the kernel is aware that a process may encapsulate several schedulable execution contexts.  
   - every thread operation must go through kernel.
   - one-size-fits-all thread implementation.
   - general heavy-weight memory requirements

1. user threads **user-managed threads** : such execution contexts are managed from a library running in user level.
   - cannot take advantage of multiple CPUs
   - a blocking system call blocks all threads(within the same process)

1. N:M threading : hybrid threading
   N user threads per M kernel threads  
   - same problem as user threads  
   - hard to keep the number of kernel  
   - threads the same as available CPUs  

1. Details  

- one thread of a program call fork(): only the calling thread is replicated in the child process
- one thread of a program call exec(): the program replaces the entire process including all threads  
- cancellation : 1 before it has completed 2 asynchronous cancellation 3 deferred cancellation
- only a single thread receives a given signal instance within a process
- **all threads share the data of the enclosing process** qnd maybe each thread need to have its own copy of certain data.

# Lecture 5 Virtual Memory

## Issues in sharing physical memory

1. **protection** : a bug in one process can corrupt memory in another
2. **transparency** : a process should not require fixed memory locations
3. **resources exhaustion** : the sum of sizes of all processes is greater than physical memory

## Goal of virtual memory

1. give each program its own "virtual" address space
2. enforce protection
3. allow programs to see more memory than exists

## Advantage

1. relocate program while running
2. most of the memory of a process is idle

## Implement ways

 Idea 1 load-time linking  
  keep the list of the memory references  
  
 1. how to enforce protection :  
 2. how to move data during execution(after startup) :  
 3. what if no contiguous free region fits program :  

 Idea 2 base + bound registers  

 1. phys.addr = virt.addr. +base
  check  0 <= virt.addr. < bound else trap to kernel  
 2. move a process in memory : change base register
 3. context switch: OS must reload/modify base and bound register

 advantage: cheap to implement in hardware ; cheap in terms of cycles  
 disadvantage: ***growing*** the memory of a process is expensive ; ***no way to share code or data***

## Virtual memory definitions

 1. programs manipulate **virtual/logical** addresses
 2. actual memory uses **physical/real** addresses
 3. hardware uses  **Memory Management Unit** MMU  
    - a part of CPU
    - accessed with privileged instructions
    - translates from virtual to physical addresses
    - provides address space

## Segmentation

 1. let processes have many base/bound registers 
 2. advantage:
    multiple segments per process
    allow sharing :

    disadvantage:
    - requires translation hardware performance limit
    - N-byte segment needs N contiguous bytes of physical memory
    - fragmentation : variable-sized pieces --**external fragmentation**
      fixed-sized pieces no external holes but force internal waste**internal fragmentation**

## Page

  1. divide memory into fixed-size pages
  2. map virtual pages to physical pages **frame** each process has separate mapping configuration  
  3. allow os to gain control on certain operations
  4. trade-offs:
    + eliminates external fragmentation
    + simplifies allocation, free, backing storage
    + average internal fragmentation of 0.5 pages per segment
  
  5. data structure
    + ***pages are fixed size e.g. 4kb **12bits for 4kb-pages page offset** **page number*****
    + ***each process has a page table*** maps virtual page number to physical page number VPN to PPN, includes bits for protection, validity.
    + what happens in the case of a read access and for a write access :
