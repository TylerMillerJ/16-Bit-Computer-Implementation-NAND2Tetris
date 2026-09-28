#include <iostream>
#include <fstream>
#include <filesystem>
#include <string>

namespace fs = std::filesystem;

std::string byteToBinary(unsigned char byte) {
    std::string result;

    for (int bit = 7; bit >= 0; --bit) {
        result += (byte & (1 << bit)) ? '1' : '0';
    }

    return result;
}

int main() {
    /*
        Expected structure:

        tests/
        ├── assembler-outputs/
        │   ├── ASCII files/
        │   └── binary-files/
        ├── expected-outputs/
        └── tools/
            └── binaryToText
    */

    fs::path toolsDirectory = fs::current_path();
    fs::path testsDirectory = toolsDirectory.parent_path();

    fs::path binaryDirectory =
        testsDirectory / "assembler-outputs" / "binary-files";

    fs::path asciiDirectory =
        testsDirectory / "assembler-outputs" / "ASCII files";

    // Create the ASCII files directory if it doesn't exist.
    fs::create_directories(asciiDirectory);

    if (!fs::exists(binaryDirectory) ||
        !fs::is_directory(binaryDirectory)) {

        std::cerr
            << "Error: Could not find binary-files directory:\n"
            << binaryDirectory << '\n';

        return 1;
    }

    for (const auto& entry :
         fs::directory_iterator(binaryDirectory)) {

        if (!entry.is_regular_file()) {
            continue;
        }

        if (entry.path().extension() != ".hack") {
            continue;
        }

        fs::path inputFile = entry.path();

        std::string baseName =
            inputFile.stem().string();

        fs::path outputFile =
            asciiDirectory /
            (baseName + "Assembled.txt");

        // Don't overwrite an existing conversion.
        if (fs::exists(outputFile)) {
            std::cout
                << "Skipping "
                << inputFile.filename()
                << " - "
                << outputFile.filename()
                << " already exists.\n";

            continue;
        }

        // Open the .hack file as raw binary.
        std::ifstream input(
            inputFile,
            std::ios::binary
        );

        if (!input.is_open()) {
            std::cerr
                << "Error: Could not open "
                << inputFile
                << '\n';

            continue;
        }

        std::ofstream output(outputFile);

        if (!output.is_open()) {
            std::cerr
                << "Error: Could not create "
                << outputFile
                << '\n';

            continue;
        }

        unsigned char byte1;
        unsigned char byte2;

        int instructionCount = 0;
        bool firstInstruction = true;

        while (true) {

            // Read first 8 bits.
            input.read(
                reinterpret_cast<char*>(&byte1),
                1
            );

            if (!input) {
                break;
            }

            // Read second 8 bits.
            input.read(
                reinterpret_cast<char*>(&byte2),
                1
            );

            if (!input) {
                std::cerr
                    << "Warning: "
                    << inputFile.filename()
                    << " contains an incomplete 16-bit instruction.\n";

                break;
            }

            // Put newlines BETWEEN instructions,
            // not after the final instruction.
            if (!firstInstruction) {
                output << '\n';
            }

            output << byteToBinary(byte1)
                   << byteToBinary(byte2);

            firstInstruction = false;
            instructionCount++;
        }

        input.close();
        output.close();

        std::cout
            << "Converted "
            << inputFile.filename()
            << " -> "
            << outputFile.filename()
            << " ("
            << instructionCount
            << " instructions)\n";
    }

    std::cout << "\nConversion complete.\n";

    return 0;
}