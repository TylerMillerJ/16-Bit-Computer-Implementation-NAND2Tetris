# Hack Assembler

## Overview

This project is the Hack Assembler for the Nand2Tetris course. It reads a `.asm` file containing a program written in Hack Assembly code, assembles it to machine language, and outputs a binary `.hack` file. The assembler uses a two-pass architecture: the first pass builds a complete symbol table (resolving labels and pre-existing direct memory references), and the second pass translates each instruction into 16-bit Hack machine code.

The Hack platform's A-register can address a 15-bit space (2^15 = 32,768 locations), but physical RAM only spans addresses 0–24576, with the first 16 (R0–R15) pre-allocated to predefined symbols and addresses 16384–24576 reserved for the memory-mapped screen and keyboard. This leaves 16,368 addresses (16–16383) available for variable declarations. The assembler enforces this limit and throws a runtime error if a program's variable count would exceed it.

The assembler is single-threaded and was benchmarked on inputs ranging from the standard Nand2Tetris test programs to synthetic programs containing up to 50 million lines. On the largest tests, it processes several million source lines per second while maintaining relatively low memory usage. More information about the benchmark methodology, results, and scaling characteristics is provided in the [Performance / Scaling](#performance--scaling) section below.

Nand2Tetris is openly available, teaching students to build a computer, starting with implementing logic gates in HDL, and building up to writing games and an operating system which runs on the computer. For more information, the course is available at [Nand2Tetris.com](https://www.nand2tetris.org) or on Coursera.

## Build & Usage

All source files are in the `/src` folder. The Makefile builds the project and tracks dependencies, placing object and dependency files in the `/build` folder. The executable is placed in the `/Assembler` base directory. Test assembly files are in the `/test/asmfiles` folder.

Test files must be placed in the same folder as the assembler in order to work, so that the assembler can properly read the input file stream. It is run as follows:

```bash
./HackAssembler Add.asm


## Architecture

For more details about the Hack language specification and definitions of labels, symbols, and instruction types, see the [Hack Assembly Language Specification](#hack-assembly-language-specification) section below.

### Initialization

#### Symbol Table

The symbol table is used to store all unique symbols that the Hack program uses. The assembler initializes it with all pre-defined symbols, and then, as the assembler reads a program, adds all new symbols to it. Symbols are stored as strings, and their value pairs are stored as 16-bit unsigned integers, since they represent line numbers and/or memory addresses up to 2^15. As the program translates each symbol to binary, it checks if the symbol is in the table and retrieves the value stored with that symbol.

Since there are frequent lookups, I chose an `unordered_map` for its expected O(1) average lookup time, which outweighs the disadvantage of needing to use a little more memory. One consideration to look into further is that, because we don't know how many symbols the table will need to store, it may frequently resize, which could impact scaling and performance.

#### File Opening and Creation

The program begins by opening and reading the name of the input file, ensuring it is a `.asm` file, keeping its input stream open, then creating the `.hack` output file of the same basename, keeping its output stream open for writing.

The assignment intended for the output file to use ASCII characters `1` and `0`; however, I made an adjustment in order to make a real assembler which outputs binary. I also created another utility which translates the binary file into an ASCII character file, in order to test whether the output matches the test files, and to output any lines where the code differs.

#### Cleaning Lines

In both the first and second pass, the first thing done is cleaning the line, removing any whitespace or comments that may come after an instruction, or that make up the entire line. The cleaned line is what is used for the following steps.

### First Pass

The project recommended using a two-pass architecture, where the first pass identifies and saves all labels into the symbol map. This approach left a RAM collision bug, which I fixed by modifying the recommended architecture.

The bug: if the programmer wants to access a register directly using `@100` (i.e., `RAM[100]`), there was no way of knowing whether the assembler had also assigned a symbol's value to that same address, `RAM[100]`. This meant the program could overwrite data saved by variables.

The solution I implemented was to identify all A-instructions in the first pass, test whether they were strings or numbers, and, if they were numbers, store them into the symbol table and into a vector of used memory addresses.

The first pass also identifies all label declarations — all lines containing a `(` and `)`. The assembler keeps a count of the instruction line number being parsed, and when a label is identified, saves the string between the brackets into the symbol table, with the instruction number as the value. The instruction line counter skips all lines that are only whitespace, comments, or labels, since they are not real instruction lines. Label declarations (between brackets) are different from their invocation using `@LABELNAME`.

### Second Pass

The second pass, skipping empty lines and labels, identifies whether an instruction is an A-instruction by testing if `line[0] == "@"`. If it is not, it must be a C-instruction.

**If it is an A-instruction:**
- It checks if it is already in the symbol table and retrieves the value.
- If it is not in the table, it is placed into the symbol table and assigned an unused memory address, using a global counter initialized at the first value above the predefined memory addresses, checked against the used-memory vector.
- The value is then converted to binary and written to the `.hack` file.

**If it is a C-instruction:**
- C-instructions use an `unordered_map` for key lookups on the `dest` and `comp` fields. The values stored are hex values representing the specific bits that each field's string value represents. The `comp` field bits also handle setting the first 3 bits to `111`, since this is mandatory for any C-instruction.
- The `comp` field is mandatory and always present, so it is parsed and stored into a string.
- The `dest` and `jump` fields are tested for presence and, if present, stored in an `auto` variable. If not present, the variable stores `nullopt`, used for a boolean check later.
- An unsigned 16-bit int variable, `binaryCInstruction`, is declared and initialized to the `comp` field's table value for the parsed line.
- If there is a `dest` or `jump` field, `binaryCInstruction` is bitwise OR'd with their values; otherwise, those bits remain `0`.
- The `dest` field does not use an unordered map. Instead, it uses a switch statement that iterates through the characters and bitwise-ORs each character's corresponding bit from the language specification. This is because `dest` can be written using `A`, `D`, `M` in any combination or permutation, and each letter represents a specific bit.
- Once these steps are complete, the returned value is an unsigned int with all the correct bits set, which is converted to binary and written to the `.hack` file.

## Hack Assembly Language Specification

Here is an overview of the Hack Assembly specification, to better understand the assembler's architecture.

Hack Assembly has two types of instructions: A-instructions and C-instructions.

### A-Instructions

A-instructions are a 16-bit binary bus where the first bit is set to `0`, which the Hack computer's CPU recognizes as an A-instruction. They are used to set a value in the A register, using the rest of the 15 bits.

The Hack programmer can use A-instructions in several ways, best explained through "symbols." Symbols are an abstraction that makes assembly programming more readable.

There are three types of symbols:
- **Variables** — declared throughout the program, representing memory addresses the programmer wants to use.
- **Predefined symbols** — predefined memory addresses the programmer may want to use to make their programs more readable.
- **Labels** — used for goto statements, representing the line number of an instruction. A label is declared using parentheses, e.g. `(LABEL)`, to mark the instruction line to jump to, and is used in the program by setting the A register via `@LABEL`, then writing code for the jump condition.

### C-Instructions

C-instructions are a 16-bit binary bus where the first bit is set to `1`. Since the next two bits are unused, they are also set to `1`. There is a mandatory `comp` (computation) field, which defines the next 7 bits depending on the computation. Then there are two optional fields: `dest`, defining which RAM location stores the result of `comp` and ending in `=`, and `jump`, defining which comparison to use for a jump condition, such as `>`, `<`, `>=`, etc. If `dest` or `jump` are blank, the remaining 6 bits are `0`; otherwise their values correspond to the language specification table. The CPU reads these bits and uses them to run the program.

## Testing, Validation, and Performance / Scaling

### Testing

Test programs were provided by the Nand2Tetris course, with correct binary files represented using ASCII `1` and `0`. I created a program, `./binaryToText`, which converts my binary files to their ASCII equivalent, taking the `.hack` file as an argument.

I also created `./compareFiles`, which compares two files line by line, displaying the line number of any differences, taking the two files as arguments.

Test programs can be found in the `01-TestPrograms` folder. The largest provided test program from the course is `Pong.asm`, with 28,375 lines, of which 27,483 are instructions. The solution file is identical to the output from my assembler, translated to ASCII.

### Performance / Scaling

The assembler is single-threaded, so the benchmark results primarily measure the efficiency of the implementation, data structures, parsing approach, and I/O rather than parallel hardware utilization.

The benchmarks were designed to examine how the assembler behaves as input size and symbol-table usage increase. In particular, the synthetic tests were designed to approach the Hack platform's maximum number of available variable addresses while also increasing the total number of instructions by several orders of magnitude.

The standard `Pong.asm` test was benchmarked using `hyperfine` with 10 warmup runs and 1,000 benchmark runs. The assembler achieved an average execution time of approximately 1.4 ms with a standard deviation of 0.2 ms, corresponding to approximately 20.3 million input lines per second. The assembler used approximately 3,864 KB of memory.

Two larger synthetic programs were then used to examine scaling behavior.

#### 1-Million-Line Test

`LargeTest1M.asm` contains 1,000,000 lines:

- 500,000 A-instructions referencing 16,000 unique symbols.
- 500,000 random C-instructions.
- No explicit label declarations.
- All 16,000 symbols are therefore introduced implicitly as variables.

Using `hyperfine` with 10 warmup runs and 100 benchmark runs, the assembler achieved:

| Metric | Result |
|---|---:|
| Input size | 1,000,000 lines |
| Unique symbols | 16,000 |
| Average execution time | 189.9 ms |
| Standard deviation | 19.8 ms |
| Throughput | ~5.27 million lines/sec |
| Peak memory | 4,672 KB |
| Memory per input line | ~4.78 bytes |

This test primarily exercises the variable-allocation path. Because the symbols are referenced but never explicitly declared with `(LABEL)` syntax, the assembler must resolve them as variables during the second pass.

#### 50-Million-Line Test

`LargeTest50M.asm` contains 50,000,000 lines:

- 16,000 explicit label declarations.
- 24,984,000 A-instructions referencing those labels.
- 25,000,000 random C-instructions.

This test exercises both the label-parsing path and the symbol lookup path while significantly increasing the total input size.

Using `hyperfine` with 10 warmup runs and 100 benchmark runs, the assembler achieved:

| Metric | Result |
|---|---:|
| Input size | 50,000,000 lines |
| Unique symbols | 16,000 |
| Average execution time | 10.949 s |
| Standard deviation | 0.544 s |
| Throughput | ~4.57 million lines/sec |
| Peak memory | 4,824 KB |
| Memory per input line | ~0.099 bytes |

The reduction in throughput from approximately 5.27 million lines/sec on the 1-million-line test to approximately 4.57 million lines/sec on the 50-million-line test indicates that throughput is not perfectly constant as input size increases. This is expected for a workload dominated by parsing, symbol-table access, memory allocation, and file I/O.

The important scaling characteristic is that memory usage remains relatively stable as the number of input lines increases. The 1-million-line test used approximately 4.7 MB of memory, while the 50-million-line test used approximately 4.8 MB. This is largely because the memory requirements are driven by the number of unique symbols and assembler state rather than by the total number of input lines processed.

The synthetic tests use 16,000 unique symbols, which is slightly below the Hack platform's maximum of 16,368 available variable addresses. This was intentional so that the tests exercise a near-maximum symbol-table workload without exceeding the Hack memory model.

### Benchmark Environment

All benchmarks were run on a laptop with a 12th Gen Intel Core i7-1280P processor, consisting of 14 cores / 20 threads (6 performance cores and 8 efficiency cores), with a maximum clock speed of up to 4.8 GHz and 15 GB of RAM.

The system was running Linux with kernel 7.1.8 on Fedora 44.

The assembler itself is single-threaded and does not distribute the workload across the available CPU cores. Therefore, the hardware specification is provided primarily for reproducibility and context. The benchmark results should not be interpreted as a measurement of multi-core scaling or as a direct representation of the assembler's performance on other hardware.

Because the benchmark is performed on a specific system, absolute execution times and throughput will vary depending on CPU performance, memory subsystem, storage performance, operating-system scheduling, compiler configuration, and other system conditions.

The more useful scaling observations are therefore the relationship between input size, symbol count, memory usage, and execution time rather than the absolute throughput number on this particular machine.

## Limitations

- The assembler expects that the assembly file is error free and correctly written with Hack syntax.
- The assembler cannot process file paths and currently only accepts files located in the same directory as the executable.
- There is limited error handling and limited built-in testing to ensure that output files are correctly generated.
- The assembler is currently single-threaded.
- The current implementation does not support processing multiple assembly files in parallel.

## TODO

- Use object-oriented design to create well-encapsulated classes.
- Implement dead-register detection and storage reclamation.
- Reduce duplicated logic, such as cleaning source lines during both the first and second passes.
- Improve error handling.
- Investigate whether repeated line-cleaning should be replaced with a different approach. Repeating line-cleaning avoids storing cleaned lines or writing them to an intermediate file, which would increase memory usage and I/O operations.
- Allow users to provide directory paths, not only individual file paths.
- Investigate symbol-table preallocation to reduce potential `unordered_map` rehashing as the number of symbols increases.
- Investigate whether further optimizations can improve scaling on very large input files.
