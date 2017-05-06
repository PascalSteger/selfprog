SelfProg
========

Project to evolve a self-replicating program
--------------------------------------------

SelfProg is a project to emulate the evolution of a genome in assembler.

Why would we want to do this? In conventional programming, a computer program is written in a way that specifies each operation the computer has to perform. Random elements mostly apply to the data the program is working with, not the program structure itself. The algorithm can thus not change, and human interaction is needed to evolve it to a new stage.

The idea behind SelfProg is to enable the program to modify itself, randomly, and to evolve a pool of such modified programs.


Measure Intelligence
--------------------
Presented with a problem, following are signs of intelligence:

- fluency (more ideas score higher)
- elaboration (more reasons score higher)
- flexibility (more categories of ideas score higher)
- originality (ideas with smaller number of others finding them score higher)


In a first step, we need a formalism to present a "problem" to the program, other than self-reproduction: define input, e.g. additional bit at the end or completely other text input. If program crashes, it cannot handle the outside world and should be removed from the pool.
In a second step, we need to quantify the output: give more weight/reproduction weight to programs with more output/more complex output.
In a third step, we need to increase the input to pose a real problem:
- continue a repetitive series of bit inputs
- propose next bit series in a series of input bits:
  - kudos for 1 1 1 => 1
  - kudos for negative ideas
  - kudos for differing outputs after repetitive runs (may need injection of random element generator in start program, i.e. storing part of random number generator in a register and using that)


nature.cpp
----------
Program to steer modifications and recompilation check. This script intends to emulate nature in the process of evolution.

Includes a framework for feeding a program an external input file, capturing the output, and rewarding intelligence.

cell
----
cell is the main program. It started as a small compiler in assembler to transform bytes written out in a text file into a corresponding executable hex file.

recompiles
----------
Directory to hold current programs that not only execute without error message, but also are able to reproduce themselves from their byte output.

Installation
------------
Run "make clean && make" to compile, and then "./nature" from inside the directory.

TODO
----
* specify path once in the beginning
* write nature.sh as C or assembler program
* parallelize to N processors concurrently
* central database of genomes
* add criterium for fitness
* try on other architectures
* limit number of current processes
* add code to copy together from two cells


Version history
---------------
v1.0 initial start -- nature.sh






GPL v.3 2015-2017
Pascal Steger
pascal@steger.aero
