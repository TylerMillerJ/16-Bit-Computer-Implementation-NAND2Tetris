#include "process_hack_a_instructions.h"
#include "parsing_utilities.h"
#include "process_hack_a_instructions.h"
#include "second_pass.h"
#include "process_hack_c_instructions.h"
#include "symbol_table.h"

#include <fstream>
#include <string>
#include <iostream>

void completeSecondPass(std::ifstream& asmFile, std::ofstream& hackfile){
  asmFile.clear();
  asmFile.seekg(0);
  hackfile.seekp(0);
  
  std::string line;

  while(std::getline(asmFile, line)){
    line = cleanLine(line);

    if (line.empty() || isLabel(line)) {
        continue;
    } 

    if (isHackAInstruction(line)) {
        proccessHackAInstructionSecondPass(line, hackfile);
    } else {
          proccessHackCInstruction(line, hackfile);
        
    }
  }
}



