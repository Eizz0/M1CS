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
