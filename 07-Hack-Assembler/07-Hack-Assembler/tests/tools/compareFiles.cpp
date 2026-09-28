#include <iostream>
#include <fstream>
#include <filesystem>
#include <string>
#include <iomanip>
#include <vector>

namespace fs = std::filesystem;

struct ComparisonResult {
    std::string file1;
    std::string file2;

    int linesCompared = 0;
    int linesEqual = 0;
    int linesDifferent = 0;

    // Stores:
    // line number
    // line from File 1
    // line from File 2
    struct Difference {
        int lineNumber;
        std::string file1Line;
        std::string file2Line;
    };

    std::vector<Difference> differences;
};

ComparisonResult compareFiles(
    const fs::path& asciiFile,
    const fs::path& expectedFile
) {
    ComparisonResult result;

    result.file1 = asciiFile.filename().string();
    result.file2 = expectedFile.filename().string();

    std::ifstream file1(asciiFile);
    std::ifstream file2(expectedFile);

    if (!file1.is_open()) {
        std::cerr
            << "Error: Could not open "
            << asciiFile << '\n';

        return result;
    }

    if (!file2.is_open()) {
        std::cerr
            << "Error: Could not open "
            << expectedFile << '\n';

        return result;
    }

    std::string line1;
    std::string line2;

    int lineNumber = 0;

    while (true) {

        bool hasLine1 =
            static_cast<bool>(std::getline(file1, line1));

        bool hasLine2 =
            static_cast<bool>(std::getline(file2, line2));

        // Both files are finished.
        if (!hasLine1 && !hasLine2) {
            break;
        }

        lineNumber++;
        result.linesCompared++;

        // Remove Windows carriage returns if present.
        if (!line1.empty() && line1.back() == '\r') {
            line1.pop_back();
        }

        if (!line2.empty() && line2.back() == '\r') {
            line2.pop_back();
        }

        if (hasLine1 &&
            hasLine2 &&
            line1 == line2) {

            result.linesEqual++;

        } else {

            result.linesDifferent++;

            result.differences.push_back({
                lineNumber,
                hasLine1 ? line1 : "<NO LINE>",
                hasLine2 ? line2 : "<NO LINE>"
            });
        }
    }

    return result;
}

int main() {

    /*
        Expected structure:

        tests/
        ├── assembler-outputs/
        │   ├── ASCII files/
        │   │   ├── AddAssembled.txt
        │   │   └── ...
        │   └── binary-files/
        │       ├── Add.hack
        │       └── ...
        │
        ├── expected-outputs/
        │   ├── Add.txt
        │   └── ...
        │
        └── tools/
            └── compareFiles
    */

    fs::path toolsDirectory = fs::current_path();

    fs::path testsDirectory =
        toolsDirectory.parent_path();

    fs::path asciiDirectory =
        testsDirectory /
        "assembler-outputs" /
        "ASCII files";

    fs::path expectedDirectory =
        testsDirectory /
        "expected-outputs";

    if (!fs::exists(asciiDirectory) ||
        !fs::is_directory(asciiDirectory)) {

        std::cerr
            << "Error: Could not find ASCII files directory:\n"
            << asciiDirectory
            << '\n';

        return 1;
    }

    if (!fs::exists(expectedDirectory) ||
        !fs::is_directory(expectedDirectory)) {

        std::cerr
            << "Error: Could not find expected-outputs directory:\n"
            << expectedDirectory
            << '\n';

        return 1;
    }

    std::vector<ComparisonResult> results;

    /*
        Iterate through all files in:

        assembler-outputs/ASCII files/
    */

    for (const auto& entry :
         fs::directory_iterator(asciiDirectory)) {

        if (!entry.is_regular_file()) {
            continue;
        }

        if (entry.path().extension() != ".txt") {
            continue;
        }

        std::string assembledName =
            entry.path().stem().string();

        const std::string suffix = "Assembled";

        /*
            Only process files such as:

            AddAssembled.txt
            MaxAssembled.txt
            PongAssembled.txt
        */

        if (assembledName.length() < suffix.length() ||
            assembledName.substr(
                assembledName.length() - suffix.length()
            ) != suffix) {

            continue;
        }

        /*
            Remove "Assembled".

            AddAssembled -> Add
            MaxAssembled -> Max
            PongAssembled -> Pong
        */

        std::string expectedBaseName =
            assembledName.substr(
                0,
                assembledName.length() -
                suffix.length()
            );

        fs::path expectedFile =
            expectedDirectory /
            (expectedBaseName + ".txt");

        /*
            If there isn't an expected output,
            don't attempt to compare it.
        */

        if (!fs::exists(expectedFile)) {

            std::cout
                << "No expected output found for "
                << entry.path().filename()
                << " (expected "
                << expectedFile.filename()
                << ")\n";

            continue;
        }

        results.push_back(
            compareFiles(
                entry.path(),
                expectedFile
            )
        );
    }

    /*
        SUMMARY TABLE
    */

    std::cout << '\n';

    std::cout
        << std::left
        << std::setw(25)
        << "File 1"

        << std::setw(25)
        << "File 2"

        << std::right
        << std::setw(17)
        << "Lines Compared"

        << std::setw(15)
        << "Lines Equal"

        << std::setw(18)
        << "Lines Different"

        << '\n';

    std::cout
        << std::string(100, '-')
        << '\n';

    for (const auto& result : results) {

        std::cout
            << std::left
            << std::setw(25)
            << result.file1

            << std::setw(25)
            << result.file2

            << std::right
            << std::setw(17)
            << result.linesCompared

            << std::setw(15)
            << result.linesEqual

            << std::setw(18)
            << result.linesDifferent

            << '\n';
    }

    /*
        DETAILED DIFFERENCES
    */

    std::cout << "\n\n";
    std::cout << "============================================================\n";
    std::cout << "                    DIFFERENCES\n";
    std::cout << "============================================================\n";

    bool anyDifferences = false;

    for (const auto& result : results) {

        if (result.linesDifferent == 0) {
            continue;
        }

        anyDifferences = true;

        std::cout << '\n';

        std::cout
            << result.file1
            << "  vs  "
            << result.file2
            << '\n';

        std::cout
            << "------------------------------------------------------------\n";

        for (const auto& difference :
             result.differences) {

            std::cout
                << "Line "
                << difference.lineNumber
                << ":\n";

            std::cout
                << "    File 1: "
                << difference.file1Line
                << '\n';

            std::cout
                << "    File 2: "
                << difference.file2Line
                << '\n';

            std::cout << '\n';
        }
    }

    if (!anyDifferences) {

        std::cout
            << "\nNo differences found.\n";
    }

    std::cout
        << "\n";

    return 0;
}