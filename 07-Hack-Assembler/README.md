# Hack Assembler

## Overview

This project is the Hack Assembler for the Nand2Tetris course. It reads a `.asm` file containing a program written in Hack Assembly code, assembles it to machine language, and outputs a binary `.hack` file. The assembler uses a two-pass architecture: the first pass builds a complete symbol table (resolving labels and pre-existing direct memory references), and the second pass translates each instruction into 16-bit Hack machine code.

The Hack platform's A-register can address a 15-bit space (2^15 = 32,768 locations), but physical RAM only spans addresses 0–24576, with the first 16 (R0–R15) pre-allocated to predefined symbols and addresses 16384–24576 reserved for the memory-mapped screen and keyboard. This leaves 16,368 addresses (16–16383) available for variable declarations. The assembler enforces this limit and throws a runtime error if a program's variable count would exceed it.

The assembler achieves an average throughput of 5.27 million lines/second on a 1-million-line program using 16,000 unique symbols, and 4.57 million lines/second on a 50-million-line program using 16,000 unique symbols combined with explicit label declarations. More information is in the Performance section below.

The assembler is single-threaded and these figures were measured on a laptop with a 12th Gen Intel Core i7-1280P (14 cores / 20 threads — 6 performance, 8 efficiency — up to 4.8 GHz, 15 GB RAM) under Linux (kernel 7.1.8, Fedora 44).

Nand2Tetris is openly available, teaching students to build a computer, starting with implementing logic gates in HDL, and building up to writing games and an operating system which runs on the computer. For more information, the course is available at [Nand2Tetris.com](https://www.nand2tetris.org) or on Coursera.

## Architecture

For more details about the Hack language specification and definitions of labels, symbols, and instruction types, see the [Hack Assembly Language Specification](#hack-assembly-language-specification) section below.

### Initialization

#### Symbol Table

The symbol table is used to store all unique symbols that the Hack program uses. The assembler initializes it with all pre-defined symbols, and then, as the assembler reads a program, adds all new symbols to it. Symbols are stored as strings, and their value pairs are stored as 16-bit unsigned integers, since they represent line numbers and/or memory addresses up to 2^15. As the program translates each symbol to binary, it checks if the symbol is in the table and retrieves the value stored with that symbol.

Since there are frequent lookups, I chose an `unordered_map` for its quick O(1) lookup time, which outweighs the disadvantage of needing to use a little more memory. One consideration to look into further is that, because we don't know how many symbols the table will need to store, it may frequently resize, which could impact performance.

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
- **Predefined symbols** — predefined memory addresses the programmer may want to use to make their program more readable.
- **Labels** — used for goto statements, representing the line number of an instruction. A label is declared using parentheses, e.g. `(LABEL)`, to mark the instruction line to jump to, and is used in the program by setting the A register via `@LABEL`, then writing code for the jump condition.

### C-Instructions

C-instructions are a 16-bit binary bus where the first bit is set to `1`. Since the next two bits are unused, they are also set to `1`. There is a mandatory `comp` (computation) field, which defines the next 7 bits depending on the computation. Then there are two optional fields: `dest`, defining which RAM location stores the result of `comp` and ending in `=`, and `jump`, defining which comparison to use for a jump condition, such as `>`, `<`, `>=`, etc. If `dest` or `jump` are blank, the remaining 6 bits are `0`; otherwise their values correspond to the language specification table. The CPU reads these bits and uses them to run the program.

## Testing, Validation, and Performance

### Testing

Test programs were provided by the Nand2Tetris course, with correct binary files represented using ASCII `1` and `0`. I created a program, `./binaryToText`, which converts my binary files to their ASCII equivalent, taking the `.hack` file as an argument.

I also created `./compareFiles`, which compares two files line by line, displaying the line number of any differences, taking the two files as arguments.

Test programs can be found in the `01-TestPrograms` folder. The largest provided test program from the course is `Pong.asm`, with 28,375 lines, of which 27,483 are instructions. The solution file is identical to the output from my assembler, translated to ASCII.

### Performance

I used `hyperfine` with 10 warmup runs and 1,000 benchmark runs to test assembling `Pong.asm`. The assembler achieved an average throughput of approximately 20.3 million lines per second, corresponding to an average execution time of 1.4 ms with a standard deviation of 0.2 ms. The assembler used 3,864 KB of memory — approximately 137 bytes per line.

I benchmarked the assembler on two synthetic test files, both using 16,000 unique symbols — slightly below the maximum of 16,368 available variable addresses for a Hack program. The two files exercise different parts of the assembler: LargeTest1M.asm tests only the variable-allocation path (symbols are referenced via @symbol but never declared with (LABEL) syntax, so every symbol is treated as an implicitly-declared variable), while LargeTest50M.asm additionally includes explicit label declarations, exercising the label-parsing path as well.

LargeTest1M.asm contains 1,000,000 lines: 500,000 A-instructions referencing 16,000 unique symbols (all implicitly declared as variables, with no matching label declarations in the file), and 500,000 random C-instructions. Using hyperfine with 10 warmup runs and 100 benchmark runs, the assembler achieved an average execution time of 189.9 ms with a standard deviation of 19.8 ms, corresponding to a throughput of approximately 5.27 million lines per second. The assembler used 4,672 KB of peak memory — approximately 4.78 bytes per line.

LargeTest50M.asm contains 50,000,000 lines: 16,000 explicit label declarations, 24,984,000 A-instructions referencing those labels, and 25,000,000 random C-instructions. Because the symbols here are declared with (LABEL) syntax rather than being introduced implicitly through use, this test additionally exercises the label-parsing path that LargeTest1M.asm does not. Using hyperfine with 10 warmup runs and 100 benchmark runs, the assembler achieved an average execution time of 10.949 seconds with a standard deviation of 0.544 seconds, corresponding to a throughput of approximately 4.57 million lines per second. The assembler used 4,824 KB of peak memory — approximately 0.099 bytes per line.

The assembler is single-threaded and these figures were measured on a laptop with a 12th Gen Intel Core i7-1280P (14 cores / 20 threads — 6 performance, 8 efficiency — up to 4.8 GHz, 15 GB RAM) under Linux (kernel 7.1.8, Fedora 44). The full specification is listed for reproducibility, not because the workload was parallelized across it.

## Limitations
- The assembler expects that the assembly file is error free and correctly written with Hack syntax

-The assembler cannot proccess file paths, only direct files within the same directory as the exectuable

- There is limited error handling and built in testing to ensure that the output files are correctly built


## TODO

- Use object-oriented design to create well-encapsulated classes.
- Implement dead-register detection and storage reclamation.
- Reduce duplicated logic, such as cleaning source lines during both the first and second passes.
- Improve Error Handling
  - Repeating line-cleaning may be preferable to storing cleaned lines or writing them to an intermediate file, since doing so would increase memory usage and I/O operations.
- Allow users to provide directory paths, not only individual file paths.
- Implement a multiprocessing architecture:
  - One process identifies the type of each instruction line.
  - The line is then dispatched to the appropriate processing worker.
- Optimize the symbol table using multiprocessing and sharding.


#### Method: Multi-Processing and Multi-Threading Hash Table Sharding Preliminary Test Results

I tested a range of process and thread configurations, using input files ranging from 10,000 to 5 million lines, where each line contained a `<symbol, value>` pair stored in an `unordered_map`. The primary test file contained 1,000,000 lines and 432,220 unique symbols, meaning the workload contained a significant number of duplicate symbols.

Initial tests showed that simply adding processes and threads did not necessarily improve performance. I tested configurations where multiple workers inserted values into a shared global `unordered_map`, as well as configurations where each worker maintained its own `unordered_map`, merged at the end. The shared-map approach introduced significant contention, while the separate-map approach introduced overhead from maintaining and merging multiple symbol tables. In both cases, the overhead outweighed the benefits of parallelizing file processing and resulted in performance worse than the single-process baseline.

I then implemented **hash sharding**, where the hash of each symbol determines which worker is responsible for it. For example, with 64 workers, a symbol is assigned via `hash(symbol) % 64`, giving each worker ownership of its own `unordered_map`. Because the same symbol always produces the same shard, all occurrences of a symbol — including duplicates — are handled by the same worker. This eliminates the need for a shared global map and avoids a final merge, since the correct map can always be determined from the symbol's hash during lookup.

Across the configurations tested, approximately 3–4 processes with 10–16 threads per process provided the best performance. The best configuration was 4 processes × 16 threads (64 total workers, 64 hash-sharded maps), achieving an average processing time of 30.30 ms, compared with 114.62 ms for the current single-process, single-map implementation — an average **3.78× speedup** for the symbol-table construction benchmark. This may also help reduce the need for rehashing tables, since sizes will be moderated, though this is a point for further investigation.

Key results:

| Configuration | Avg. Time | Speedup vs. Baseline |
|---|---|---|
| 4 processes × 16 threads → hash % 64 → 64 sharded maps | 30.30 ms | 3.78× |
| 1 process → 1 map | 114.62 ms | 1.00× |
| 1 process → 5 maps | 194.35 ms | 0.59× |
| 5 processes → 5 maps → merge | 203.01 ms | 0.56× |
| 5 processes → 1 shared/global map | 589.11 ms | 0.19× |


## Build & Usage

All source files are in the `/src` folder. The Makefile builds the project and tracks dependencies, placing object and dependency files in the `/build` folder. The executable is placed in the `/Assembler` base directory. Test assembly files are in the `/test/asmfiles` folder.

Test files must be placed in the same folder as the assembler in order to work, so that the assembler can properly read the input file stream. It is run as follows:

```
./HackAssembler Add.asm
```

This creates the `.hack` output file in the same directory as the `./HackAssembler` executable.

## Tools / Test Folder Usage

Expected outputs can be found on the Nand2Tetris online IDE, and are also included in the `/test/expected-outputs` folder.

Since this assembler outputs binary files instead of the ASCII files expected for the assignment, there is a program, `./binaryToText`, in the `/tools` folder, which iterates through the binary files folder and converts any files not already in the ASCII folder into their ASCII equivalent.

`./compareFiles` iterates through the ASCII folder and the expected-output folder, comparing files line by line and outputting details: files compared, lines read, lines matched, lines different, and total lines read. If any lines differ, the line numbers are provided.

The results of comparing all expected outputs against their assembled equivalents are in `results.txt`, or below:

| Assembled Output | Test File | Lines Compared | Lines Equal | Lines Different |
|---|---|---|---|---|
| MaxLAssembled.txt | MaxL.txt | 16 | 16 | 0 |
| PongLAssembled.txt | PongL.txt | 27,483 | 27,483 | 0 |
| MaxAssembled.txt | Max.txt | 16 | 16 | 0 |
| AddAssembled.txt | Add.txt | 6 | 6 | 0 |
| PongAssembled.txt | Pong.txt | 27,483 | 27,385 | 98 |

The `Pong.txt` file contained 98 line differences. These are attributable to a difference in assembler architecture: this version identifies all direct register accesses and ensures no symbols are assigned to those register values in the second pass, preventing register collisions. There is a 1-to-1 correspondence between the binary outputs, shown below:

| Assembled Output | Test File | Assembled Register | Test Register |
|---|---|---|---|
| 0000000000010101 | 0000000000010000 | 21 | 16 |
| 0000000000010111 | 0000000000010001 | 23 | 17 |
| 0000000000011010 | 0000000000010010 | 26 | 18 |
| 0000000000100100 | 0000000000010011 | 36 | 19 |
| 0000000000100101 | 0000000000010100 | 37 | 20 |
| 0000000000100111 | 0000000000010101 | 39 | 21 |
| 0000000000101000 | 0000000000010110 | 40 | 22 |
| 0000000000101001 | 0000000000010111 | 41 | 23 |
| 0000000000101010 | 0000000000011000 | 42 | 24 |
| 0000000000101110 | 0000000000011001 | 46 | 25 |
| 0000000000101111 | 0000000000011010 | 47 | 26 |
| 0000000000110100 | 0000000000011011 | 52 | 27 |
| 0000000000110101 | 0000000000011100 | 53 | 28 |
| 0000000000111001 | 0000000000011101 | 57 | 29 |
