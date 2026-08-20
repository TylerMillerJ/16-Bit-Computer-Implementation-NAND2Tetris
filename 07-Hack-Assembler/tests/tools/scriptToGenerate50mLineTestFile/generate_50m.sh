#!/bin/bash

#This will generate a 50 million line testfile, beware!

set -e

OUT="LargeTest50M.asm"

python3 - "$OUT" <<'PY'
import random
import sys

out = sys.argv[1]

UNIQUE_SYMBOLS = 2_500_000
SYMBOL_REFS = 22_500_000
C_INSTRUCTIONS = 25_000_000

TOTAL = UNIQUE_SYMBOLS + SYMBOL_REFS + C_INSTRUCTIONS

comps = [
    "0", "1", "-1",
    "D", "A", "!D", "!A", "-D", "-A",
    "D+1", "A+1", "D-1", "A-1",
    "D+A", "D-A", "A-D",
    "D&A", "D|A"
]

dests = ["", "M", "D", "MD", "A", "AM", "AD", "AMD"]
jumps = ["", "JGT", "JEQ", "JGE", "JLT", "JNE", "JLE", "JMP"]

def random_c_instruction():
    comp = random.choice(comps)
    dest = random.choice(dests)
    jump = random.choice(jumps)

    return (
        (dest + "=" if dest else "")
        + comp
        + (";" + jump if jump else "")
    )

print(f"Generating {TOTAL:,} lines...", file=sys.stderr)

with open(out, "w", buffering=1024 * 1024) as f:

    # 2.5 million unique label declarations
    for i in range(UNIQUE_SYMBOLS):
        f.write(f"(SYMBOL_{i})\n")

        if (i + 1) % 500_000 == 0:
            print(
                f"Labels: {i + 1:,}/{UNIQUE_SYMBOLS:,}",
                file=sys.stderr
            )

    # 22.5 million symbolic A-instructions
    for i in range(SYMBOL_REFS):
        symbol = random.randrange(UNIQUE_SYMBOLS)
        f.write(f"@SYMBOL_{symbol}\n")

        if (i + 1) % 1_000_000 == 0:
            print(
                f"Symbol references: {i + 1:,}/{SYMBOL_REFS:,}",
                file=sys.stderr
            )

    # 25 million random C-instructions
    for i in range(C_INSTRUCTIONS):
        f.write(random_c_instruction() + "\n")

        if (i + 1) % 1_000_000 == 0:
            print(
                f"C-instructions: {i + 1:,}/{C_INSTRUCTIONS:,}",
                file=sys.stderr
            )

print("Generation complete.", file=sys.stderr)
PY

echo
echo "Line count:"
wc -l "$OUT"

echo
echo "File size:"
du -h "$OUT"

echo
echo "Benchmark command:"
echo "hyperfine -w 10 -r 100 --shell=none './HackAssembler $OUT'"
