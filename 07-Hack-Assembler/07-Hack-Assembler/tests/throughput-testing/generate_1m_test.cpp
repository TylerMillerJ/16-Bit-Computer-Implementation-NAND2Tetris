// generate_1m_test.cpp
//
// Generates LargeTest1M.asm in the current directory: 1,000,000 lines,
// using 16,000 unique symbols -- just below the true maximum realistic
// symbol count for the Hack platform.
//
// NOTE ON THE MAX: physical Hack RAM only spans addresses 0-24576, not
// the full 15-bit address space (0-32767). The first 16 addresses
// (R0-R15) are reserved for predefined symbols, and addresses
// 16384-24576 are memory-mapped to the screen and keyboard, leaving
// only addresses 16-16383 (16,368 addresses) for variable declarations.
// A previous version of this generator used 30,000 unique symbols,
// which exceeds this real limit and is not achievable on actual Hack
// hardware without colliding with screen/keyboard memory.
//
//   500,000 A-instructions referencing 16,000 unique symbols
//   500,000 random valid C-instructions
//
// Build:
//   g++ -O2 -o generate_1m_test generate_1m_test.cpp
// Run:
//   ./generate_1m_test

#include <algorithm>
#include <fstream>
#include <iostream>
#include <random>
#include <string>
#include <vector>

int main() {
    const int SYMBOL_LINES = 500000;
    const int UNIQUE_SYMBOLS = 16000; // must stay below 16,368 usable addresses
    const int C_LINES = 500000;

    std::mt19937 rng(42); // seeded for reproducibility

    const std::vector<std::string> dests = {"", "M", "D", "MD", "A", "AM", "AD", "AMD"};
    const std::vector<std::string> comps = {
        "0", "1", "-1", "D", "A", "M", "!D", "!A", "!M", "-D", "-A", "-M",
        "D+1", "A+1", "M+1", "D-1", "A-1", "M-1", "D+A", "D+M", "D-A", "D-M",
        "A-D", "M-D", "D&A", "D&M", "D|A", "D|M"
    };
    const std::vector<std::string> jumps = {"", "JGT", "JEQ", "JGE", "JLT", "JNE", "JLE", "JMP"};

    std::uniform_int_distribution<int> symDist(0, UNIQUE_SYMBOLS - 1);
    std::uniform_int_distribution<int> destDist(0, static_cast<int>(dests.size()) - 1);
    std::uniform_int_distribution<int> compDist(0, static_cast<int>(comps.size()) - 1);
    std::uniform_int_distribution<int> jumpDist(0, static_cast<int>(jumps.size()) - 1);

    std::vector<std::string> lines;
    lines.reserve(SYMBOL_LINES + C_LINES);

    // A-instructions
    for (int i = 0; i < SYMBOL_LINES; ++i) {
        lines.push_back("@var" + std::to_string(symDist(rng)));
    }

    // C-instructions
    for (int i = 0; i < C_LINES; ++i) {
        const std::string& d = dests[destDist(rng)];
        const std::string& c = comps[compDist(rng)];
        const std::string& j = jumps[jumpDist(rng)];

        std::string line = c;
        if (!d.empty()) {
            line = d + "=" + line;
        }
        if (!j.empty()) {
            line = line + ";" + j;
        }
        lines.push_back(line);
    }

    // Shuffle so A- and C-instructions are interleaved
    std::shuffle(lines.begin(), lines.end(), rng);

    std::ofstream out("LargeTest1M.asm");
    if (!out) {
        std::cerr << "Error: could not open LargeTest1M.asm for writing.\n";
        return 1;
    }
    for (const auto& line : lines) {
        out << line << "\n";
    }
    out.close();

    std::cout << "Wrote " << lines.size() << " lines to LargeTest1M.asm\n";
    std::cout << "  A-instructions (symbol-related): " << SYMBOL_LINES << "\n";
    std::cout << "  Unique symbols: " << UNIQUE_SYMBOLS << "\n";
    std::cout << "  C-instructions: " << C_LINES << "\n";

    return 0;
}
