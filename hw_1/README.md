# Advanced Topics Ex1 - Game Build Instructions

## This project is submitted by

Ishay Yemini 322868852

Rachel Ganem *********

## Requirements

CMake version 3.10 or higher is required.

C++20 or higher (equivalent to a recent version of the g++ compiler).

## Note

The compiler flags added in CMakeLists.txt are the required flags and are equivalent to:

```bash

g++ -std=c++20 -Wall -Wextra -Werror -pedantic
```

## General Instructions

The following instructions are for linux / Unix based systems.

Open a terminal and navigate to the advanced_topics_ex1 directory:

```bash

cd /path/to/advanced_topics_ex1
```

Build the project for Linux based on the following instructions. Then run the project based on the "Run
instructions"

Just run:

```bash

make 
```

## Run instructions

Linux:

```bash

./tanks_game <Map_File_Name>
```

Optionally, if you want to run it with a visualization, run:

```bash

./tanks_game -g <Map_File_Name>
```

If you want to test it, run:

```bash

make test
```

