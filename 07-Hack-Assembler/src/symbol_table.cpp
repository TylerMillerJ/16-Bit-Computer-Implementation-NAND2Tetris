#include "memory_validation.h"
#include "symbol_table.h"

#include <unordered_map>
#include <string>
#include <cstdint>
#include <iostream>

using std::uint16_t;




std::unordered_map<std::string, std::uint16_t> symbolTable { 
   {"R0", 0},
   {"R1", 1},
   {"R2", 2},
   {"R3", 3},
   {"R4", 4},
   {"R5", 5},
   {"R6", 6},
   {"R7", 7},
   {"R8", 8},
   {"R9", 9},
   {"R10", 10},
   {"R11", 11},
   {"R12", 12},
   {"R13", 13},
   {"R14", 14},
   {"R15", 15},
   {"SCREEN", SCREEN_MEMORY},
   {"KBD", KBD_MEMORY},
   {"SP", 0},
   {"LCL", 1},
   {"ARG", 2},
   {"THIS", 3},
   {"THAT", 4}
   };


uint16_t findSymbolTableValue(std::string& line){
        auto valueLocationIterator = symbolTable.find(line);
        uint16_t symbolTableValue;

        if (valueLocationIterator != symbolTable.end()){ //if the symbol already exists in the table
            symbolTableValue = valueLocationIterator->second;
        } else {
            symbolTableValue = assignValidMemoryAddress();
            symbolTable.emplace(line, symbolTableValue);
        }
        return symbolTableValue;
 }


bool addedLabelsToSymbolTable(const std::string& line, const uint16_t& instructionLineNumber){

    auto positionOfStartLabelIdentifier= line.find("(");
    auto PositionOfEndLabelIdentifier = line.find(")");

    if (positionOfStartLabelIdentifier != std::string::npos && PositionOfEndLabelIdentifier != std::string::npos){ //then there is a label

        auto startOfLabelText = positionOfStartLabelIdentifier + 1; 
        auto endOfLabelText = PositionOfEndLabelIdentifier - positionOfStartLabelIdentifier - 1;

        std::string label = line.substr(startOfLabelText, endOfLabelText); 

        if (!symbolTable.insert({label, instructionLineNumber}).second){
            std::cerr << "Error: Label insert failed in first pass reading instruction line number: " << instructionLineNumber << " - Multiple label declarations of the same name may have caused this error." << std::endl;
        }
            
        return true;
    }
    return false;
}

bool isLabel(const std::string& line){
    auto positionOfStartLabelIdentifier= line.find("(");
    auto PositionOfEndLabelIdentifier = line.find(")");

        if (positionOfStartLabelIdentifier != std::string::npos && PositionOfEndLabelIdentifier != std::string::npos){ 
            return true;
        }
    return false;
}

