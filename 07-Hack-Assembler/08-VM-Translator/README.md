# Hack Assembler
This Project is currently: In Progress

## Overview

This project is the VM translator, used to translate VM code to Hack assembly.


This is built for the Nand2Tetris course Nand2Tetris is openly available, teaching students to build a computer, starting with implementing logic gates in HDL, and building up to writing games and an operating system which runs on the computer. For more information, the course is available at [Nand2Tetris.com](https://www.nand2tetris.org) or on Coursera.

## Architecture


## Hack Assembly Language Specification

Here is an overview of the Hack Assembly specification, to better understand the VM translator's architecture.

Hack Assembly has two types of instructions: A-instructions and C-instructions.

### A-Instructions

A-instructions are used to set the A Register to a value/address.

There are three types of symbols:
- **Variables** — declared throughout the program, representing memory addresses the programmer wants to use.
- **Predefined symbols** — predefined memory addresses the programmer may want to use to make their program more readable.
- **Labels** — used for goto statements, representing the line number of an instruction. A label is declared using parentheses, e.g. `(LABEL)`, to mark the instruction line to jump to, and is used in the program by setting the A register via `@LABEL`, then writing code for the jump condition.

### C-Instructions


## Testing, Validation, and Performance

### Testing


### Performance


## Limitations


## TODO


## Build & Usage



## Tools / Test Folder Usage

