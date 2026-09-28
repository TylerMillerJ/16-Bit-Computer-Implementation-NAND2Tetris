// generate_50m_test.cpp
//
// Generates LargeTest50M.asm in the current directory: 50,000,000 lines.
// BEWARE: this produces a large file and can take a while to run.
//
// Uses 16,000 unique symbols -- just below the true maximum realistic
// symbol count for the Hack platform. Physical Hack RAM only spans
// addresses 0-24576 (not the full 15-bit space, 0-32767): the first 16
// addresses (R0-R15) are reserved for predefined symbols, and addresses
// 16384-24576 are memory-mapped to the screen and keyboard. That leaves
// only 16,368 addresses (16-16383) available for variable declarations.
// A previous version of this generator used 30,000 unique symbols,
// which exceeds this real limit and cannot be represented on actual
// Hack hardware without colliding with screen/keyboard memory.
//
//   16,000 unique label declarations
//   25,484,000 symbolic A-instructions (referencing those labels)
//   25,000,000 random C-instructions
//
// Build:
//   g++ -O2 -o generate_50m_test generate_50m_test.cpp
// Run:
//   ./generate_50m_test
//
// After generating, benchmark with:
//   hyperfine -w 10 -r 100 --shell=none './HackAssembler LargeTest50M.asm'

#include <fstream>
#include <iostream>
#include <random>
#include <string>
#include <vector>

int main() {
    const char* OUT = "LargeTest50M.asm";

    const long long UNIQUE_SYMBOLS = 16'000;
    const long long SYMBOL_REFS = 24'984'000;
    const long long C_INSTRUCTIONS = 25'000'000;
    const long long TOTAL = UNIQUE_SYMBOLS + SYMBOL_REFS + C_INSTRUCTIONS;

    const std::vector<std::string> comps = {
        "0", "1", "-1",
        "D", "A", "!D", "!A", "-D", "-A",
        "D+1", "A+1", "D-1", "A-1",
        "D+A", "D-A", "A-D",
        "D&A", "D|A"
    };
    const std::vector<std::string> dests = {"", "M", "D", "MD", "A", "AM", "AD", "AMD"};
    const std::vector<std::string> jumps = {"", "JGT", "JEQ", "JGE", "JLT", "JNE", "JLE", "JMP"};

    std::mt19937 rng(42); // seeded for reproducibility
    std::uniform_int_distribution<long long> symDist(0, UNIQUE_SYMBOLS - 1);
    std::uniform_int_distribution<int> compDist(0, static_cast<int>(comps.size()) - 1);
    std::uniform_int_distribution<int> destDist(0, static_cast<int>(dests.size()) - 1);
    std::uniform_int_distribution<int> jumpDist(0, static_cast<int>(jumps.size()) - 1);

    std::cerr << "Generating " << TOTAL << " lines...\n";

    // Large output buffer to reduce I/O overhead at this scale
    std::ofstream out(OUT);
    if (!out) {
        std::cerr << "Error: could not open " << OUT << " for writing.\n";
        return 1;
    }
    std::vector<char> buf(1 << 20); // 1 MB buffer
    out.rdbuf()->pubsetbuf(buf.data(), buf.size());

    // 16,000 unique label declarations
    for (long long i = 0; i < UNIQUE_SYMBOLS; ++i) {
        out << "(SYMBOL_" << i << ")\n";
        if ((i + 1) % 500'000 == 0) {
            std::cerr << "Labels: " << (i + 1) << "/" << UNIQUE_SYMBOLS << "\n";
        }
    }

    // ~25 million symbolic A-instructions
    for (long long i = 0; i < SYMBOL_REFS; ++i) {
        long long symbol = symDist(rng);
        out << "@SYMBOL_" << symbol << "\n";
        if ((i + 1) % 1'000'000 == 0) {
            std::cerr << "Symbol references: " << (i + 1) << "/" << SYMBOL_REFS << "\n";
        }
    }

    // 25 million random C-instructions
    for (long long i = 0; i < C_INSTRUCTIONS; ++i) {
        const std::string& d = dests[destDist(rng)];
        const std::string& c = comps[compDist(rng)];
        const std::string& j = jumps[jumpDist(rng)];

        if (!d.empty()) {
            out << d << "=";
        }
        out << c;
        if (!j.empty()) {
            out << ";" << j;
        }
        out << "\n";

        if ((i + 1) % 1'000'000 == 0) {
            std::cerr << "C-instructions: " << (i + 1) << "/" << C_INSTRUCTIONS << "\n";
        }
    }

    out.close();
    std::cerr << "Generation complete.\n";

    std::cout << "\nBenchmark command:\n";
    std::cout << "hyperfine -w 10 -r 100 --shell=none './HackAssembler " << OUT << "'\n";

    return 0;
}
