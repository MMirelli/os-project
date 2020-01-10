# README
This simple project comprises three simple exercises, briefly discussed below. Unfortunately comments are in Italian, since I worked on this project at Florence University.

The [first exercise](./ex1) is a simple process scheduler; a list of records which can be dynamically allocated and modified, using the options displayed in the main menu. The main point of the exercise is using dynamic memory and pointers.

The [second exercise](./ex2) is a basic multi-command executor. The user inserts shell commands to standard input and selects the execution mode, which might be either sequential or parallel (the main process spawns as many children as the number of inserted commands). Here the aim is managing multiple processes.

The [third exercise](./ex3) is a simple message delivery system, where "clients" send messages to a "server", which forwards them to other subscribed clients. The actual message forwarding is operated by using pipes and this is the goal of the exercise.

## Project structure

[`sMakefile`](./sMakefile) contains code used in the build of all the exercises.
[`sInclude`](./sInclude) and [`sSrc`](./sSrc) are used to implement the list used both in `ex1` and `ex3`.

Each exercise comprises the usual subtree (`bin`,`include` and `src`) as shown below.

```
|   sMakefile
|   README.md
|
+---ex1
|   |   makefile
|   |
|   +---bin
|   +---include
|   |       schedulerProcessi.h
|   |
|   \---src
|           schedulerProcessi.c
|
+---ex2
|   |   makefile
|   |
|   +---bin
|   +---include
|   |       esecutoreComandi.h
|   |
|   +---output
|   \---src
|           esecutoreComandi.c
|
+---ex3
|   |   makefile
|   |
|   +---bin
|   |   \---pipes
|   +---include
|   |       client.h
|   |       server.h
|   |       util.h
|   |
|   \---src
|           client.c
|           server.c
|           util.c
|
+---sInclude
|       list.h
|
\---sSrc
        list.c
```

## Compilation instructions

Choose the i-th exercise and execute:

`cd exi`

`make`

Find the binaries in `bin`.