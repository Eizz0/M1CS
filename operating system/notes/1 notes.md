
- [Lecture 1 Introduction](#lecture-1-introduction)
  - [1 Process](#1-process)
  - [2 Protection and resource management](#2-protection-and-resource-management)
  - [3 Main techniques for virtualization](#3-main-techniques-for-virtualization)
  - [4 Exercise for lecture 1](#4-exercise-for-lecture-1)
    - [1](#1)
    - [2](#2)
    - [3](#3)
    - [4](#4)
    - [5](#5)
- [Lecture 2 Memory Management free space and dynamic allocation](#lecture-2-memory-management-free-space-and-dynamic-allocation)
  - [1 Fragmentation](#1-fragmentation)
  - [2 How to implement a memory allocator](#2-how-to-implement-a-memory-allocator)
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

### 1  

![Exercise 1](https://raw.githubusercontent.com/Eizz0/cloudimg/master/data/1.jpg)

The typical duration of 10ms for the time quantum of an operating system scheduler is motivated by the fact that, on most machines, the hardware cannot generate timer interrupts with a higher frequency. ***It' false because***

### 2

![Exercise 2](https://raw.githubusercontent.com/Eizz0/cloudimg/master/data/3.jpg)

c) blocking hardware interrupts is allowed  

d) triggering the execution of the process scheduler is allowed ***i.e. exit(0), sleep()***

### 3  

![Exercise 3](https://raw.githubusercontent.com/Eizz0/cloudimg/master/data/2.jpg)

The first design is fragile and will waste of memory.  
The second one is better in safety and maintainability.Easy to change and can be adapted to new CPU.  

### 4

Give an example of physical resource that is both time shared and space shared by an operating system.  

Disk Memory CPU  

### 5  

process and child process  
```
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

+ External Fragmentation  
when there is enough aggregate heap memory, but no single free block is large enough.

+ Internal Fragmentation  
For a given block, inter fragmentation occurs if payload is smaller than block size. **1 overhead of maintaining heap data structures 2 padding for alignment purposes 3 explicit policy decisions**  

## 2 How to implement a memory allocator  

+ Free block organization : use "free list" -- linked list of descriptors of free blocks. ***Bookkeeping(store size of block + pointers to next and previous elements in free list) + memory alignment***
  
+ Placement : the strategies have a major impact on external fragmentation.  
     1. best fit : minimize fragmentation by allocating space from block that ***leaves smallest fragment***sawdust everywhere (as a result of small remainder)  
        a list of free blocks each has a header holding block size and pointer to next
     2. first fit : ***pick the first block that fits*** 

+ Splitting :
  1. keep the remainder within the chosen block ***simple and fast but introduces more internal fragmentation***  
  2. split the chosen block in two and insert the remainder block in the free list  

+ Coalescing : helps avoiding "false external fragmentation" ; Immediate or Deferred.

<mark><b>Impossible to solve fragmentation</mark>  

> there exists streams of allocation and de-allocation requests that defeat the allocator and force it into severe fragmentation 