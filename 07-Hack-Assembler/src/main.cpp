#include <iostream>
#include <fstream>

#include "command_line_argument_processing.h"
#include "first_pass.h"
#include "parsing_utilities.h"
#include "process_hack_a_instructions.h"
#include "second_pass.h"
#include "symbol_table.h"


using std::uint16_t;


int main (int argc, char* argv[]){

 if (argc != 2) {
       std::cerr << "Usage: " << argv[0] << " <input.asm>" << std::endl;
    return 1;
}

std::ofstream hackfile = takeCommandLineArgumentAndReturnOpenHackFileStream(argv[1]);

//open the .asm file before the first pass
std::ifstream openedFileASM(argv[1]);

//First Pass
completeFirstPass(openedFileASM);

//second pass
completeSecondPass(openedFileASM, hackfile);

hackfile.close();

std::cout << "Completed" << std::endl;

return 0;
}
