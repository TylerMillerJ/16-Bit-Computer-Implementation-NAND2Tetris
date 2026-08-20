#include "first_pass.h"
#include "parsing_utilities.h"
#include "process_hack_a_instructions.h"
#include "symbol_table.h"

#include <cstdint>
#include <fstream>
#include <string>

using std::uint16_t;


void completeFirstPass(std::ifstream& asmFile){
    uint16_t instructionLineCount = 0; //returning line count, and storing it for label symbols.
    std::string line;
    asmFile.seekg(0); // for good measure
    

    while(std::getline(asmFile, line)){
        line = cleanLine(line);
            if (line.empty()){ 
                continue; //don't waste time on more checks, it was a blank line or comment
            }
            if (addedLabelsToSymbolTable(line, instructionLineCount)){ 
                continue; //dont count lines that are labels as an instruction
            }
            if (isHackAInstruction(line)){
                proccessHackAInstrutionFirstPass(line);
            }
        instructionLineCount++;
    }
}
