# Advanced Topics Ex3 - Game Build Instructions

## This project is submitted by

Ishay Yemini 322868852

Rachel Ganem 340849710

## Requirements

CMake version 3.10 or higher is required.

C++20 or higher (equivalent to a recent version of the g++ compiler).

## General Instructions

The following instructions are for linux / Unix based systems.

Open a terminal and navigate to the root directory.

Build the project for Linux based on the following instructions. Then run the project based on the "Run
instructions"

Just run:

```bash

make 
```

This will make all three subfolders.

## Run instructions

For comparative mode:

```bash

./Simulator/simulator_322868852_340849710 -comparative game_map=<file> game_managers_folder=<folder> algorithm1=<file> algorithm2=<file> [num_threads=<num>] [-verbose]
```

For competition mode:

```bash

./Simulator/simulator_322868852_340849710 -competition game_maps_folder=<folder> game_manager=<file> algorithms_folder=<folder> [num_threads=<num>] [-verbose]
```
