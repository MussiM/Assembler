# Assembler
Final project in course Linux Operating System in C.

## About The Project
The goal of the project is an assembler script - a program that translates into machine language - for an assembly language that was specifically defined for the project.

The assembler gets two files: one file with correct assembly code and a second file with incorrect assembly code.

For correct files the assembler script creates files:
1. OB file - Contains the machine code.
2. EXT file - Contains the details of all addresses that have an information word that encodes a value of a symbol declared as external, Only if there is external symbols.
3. ENT file - Contains the details of each symbol declared as an entry point, Only if there is enternal symbols.

For incorrect files the assembler script prints error masseges.

## Built With
* C

### Prerequisites
* C compiler

### Installation

1. Clone the repo
   ```sh
   git clone https://github.com/MussiM/Assembler.git
   ```
2. Open C compiler in directory and run this command to compile:
   ```sh
   gcc main.c Assembler.c Instructions.c Operands.c Symbols.c Tokens.c
   ```
3. Run this command to start the project:
   ```js
   ./a correctInput wrongInput
   ```
