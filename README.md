SelfProg
========

Project to evolve a self-replicating program
--------------------------------------------

SelfProg is a project to emulate the evolution of a genome in assembler.

Why would we want to do this? In conventional programming, a computer program is written in a way that specifies each operation the computer has to perform. Random elements mostly apply to the data the program is working with, not the program structure itself. The algorithm can thus not change, and human interaction is needed to evolve it to a new stage.

The idea behind SelfProg is to enable the program to modify itself, randomly, and to evolve a pool of such modified programs.



nature.sh
---------
Linux-shell based program to steer modifications and recompilation check. This script intends to emulate nature in the process of evolution.


cell
----
cell is the main program. It started as a small compiler in assembler to transform bytes written out in a text file into a corresponding executable hex file.

recompiles
----------
Directory to hold current programs that not only execute without error message, but also are able to reproduce themselves from their byte output.



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






GPL v.3 2015
Pascal Steger
pascal@steger.aero
