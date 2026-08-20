#include "memory_validation.h"
#include "process_hack_a_instructions.h"
#include "symbol_table.h"


#include <bit>
#include <string>
#include <fstream>
#include <cstdint>
#include <optional>
#include <iostream>
#include <charconv>

using std::uint16_t;



void proccessHackAInstrutionFirstPass(std::string& line){
    line = removeHackADeclarationSymbol(line);
    auto registerAddress = isWholeNumber(line);

    if (registerAddress && isValidAddress(*registerAddress)){
        if (!symbolTable.emplace(line, *registerAddress).second) {
            //save it so we dont assign any symbols this register in the second pass
            directlyUsedAddresses.push_back(*registerAddress);
        }
    }
}

void proccessHackAInstructionSecondPass(std::string& line, std::ofstream& hackfile){
    line = removeHackADeclarationSymbol(line);
    uint16_t symbolTableValue = findSymbolTableValue(line);
    writeSymbolValueAsBinaryToHackFile(symbolTableValue, hackfile);
}



void writeSymbolValueAsBinaryToHackFile(uint16_t& symbolTableValue, std::ofstream& hackfile){

    if constexpr (std::endian::native == std::endian::little){ //the compiler will only keep this if it turns out the host machine is little endian
        symbolTableValue = std::byteswap(symbolTableValue);          //because every value will need to be converted to big Endian
    }
    /*
    if (symbolTableValue & 0x8000){
        std::cerr << "Error: Hack A Instruction Identified with Most Signifigant Bit set to 1 - Expects 0: " << symbolTableValue << std::endl;
    }
*/
    hackfile.write(reinterpret_cast<const char*> (&symbolTableValue), 2);
}


bool isHackAInstruction(const std::string& line){
  return (line[0] == '@');
}

std::string removeHackADeclarationSymbol(const std::string& line){
    return line.substr(1);

}

std::optional<std::uint16_t> isWholeNumber(const std::string& line){
    uint16_t registerAddress;
    auto [endPointer, errorCode] = std::from_chars(line.data(), line.data() + line.size(), registerAddress);
    
    bool parsingSucceeded = (errorCode == std::errc());
    bool entireStringWasRead = (endPointer == line.data() + line.size());

    if ((parsingSucceeded && entireStringWasRead)){
        return registerAddress;
    }
    return std::nullopt;
}
