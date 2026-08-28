#include "process_hack_c_instructions.h"

#include <cstdint>
#include <string>
#include <unordered_map>
#include <optional>
#include <fstream>
#include <bit>
#include <charconv>
#include <iostream>


using std::uint16_t;

const std::unordered_map<std::string, uint16_t> compBitsTable{
    //comp field is mandatory, therefor this table will also initialize the first 3 bits for c instructions.
    // each key will set first 3 bits, a bit, and comp bits. See HACK Language Specification for more details.
    {"0", 0xEA80},
    {"1", 0xEFC0},
    {"-1", 0xEE80},
    {"D", 0xE300},
    {"A", 0xEC00},
    {"!D", 0xE340},
    {"!A", 0xEC40},
    {"-D", 0xEC40},
    {"-A", 0xECC0},
    {"D+1", 0xE7C0},
    {"A+1", 0xEDC0},
    {"D-1", 0xE380},
    {"A-1", 0xEC80},
    {"D+A", 0xE080},
    {"D-A", 0xE4C0},
    {"A-D", 0xE1C0},
    {"D&A", 0xE000},
    {"D|A", 0xE540},
    {"M", 0xFC00},
    {"!M", 0xFC40},
    {"-M", 0xFCC0},
    {"M+1", 0xFDC0},
    {"M-1", 0xFC80},
    {"D+M", 0xF080},
    {"D-M", 0xF4C0},
    {"M-D", 0xF1C0},
    {"D&M", 0xF000},
    {"D|M", 0xF540}
};


const std::unordered_map<std::string, uint16_t> jumpBitsTable{
    {"JGT", 0x1},
    {"JEQ", 0x2},
    {"JGE", 0x3},
    {"JLT", 0x4},
    {"JNE", 0x5},
    {"JLE", 0x6},
    {"JMP", 0x7},
};


void proccessHackCInstruction(std::string& line, std::ofstream& hackfile){

    uint16_t binaryToOutput = buildBinaryOutput(line);
    writeBinaryToFile(binaryToOutput, hackfile);
}

uint16_t buildBinaryOutput(const std::string& line){

    std::string compField = parseCompField(line);
    auto destField = parseDestField(line);
    auto jumpField = parseJumpField(line);

    uint16_t binaryCInstruction = compBitsTable.at(compField);

    if (destField){
            binaryCInstruction |= setDestBits(*destField);
        }
    if (jumpField){
            binaryCInstruction |= jumpBitsTable.at(*jumpField);
        }

    return binaryCInstruction;
}

void writeBinaryToFile(uint16_t& binaryCInstruction, std::ofstream& hackfile){

    if constexpr (std::endian::native == std::endian::little){          //the compiler will only keep this if it turns out the host machine is little endian
            binaryCInstruction = std::byteswap(binaryCInstruction);          //then convert to big Endian for .hack
        }
    
    hackfile.write(reinterpret_cast<const char*> (&binaryCInstruction), 2);
}

std::string parseCompField(std::string line){
    //this creates redudant work, but keeps the code seperated and simpler overall
    //Possible to explore if creating boolean comparisons in buildBinaryOutput to decide wether to parse substrings increases performance
    
    auto compFieldBeginning = line.find("=");
    
    if (compFieldBeginning != std::string::npos){
        line = line.substr(compFieldBeginning + 1);         //if present, strip it off, and everything before it
    }
 
    auto compFieldEnd = line.find(";");

    if (compFieldEnd != std::string::npos){
        line = line.substr(0, compFieldEnd);              //if present, strip it off, and everything after it
    }

    return line;
}


std::optional<std::string> parseDestField(const std::string& line){ 
    auto destFieldEndMarker = line.find("=");

    if (destFieldEndMarker != std::string::npos){
        return line.substr(0, destFieldEndMarker);
    }
    else {
        return std::nullopt;
    }
}
uint16_t setDestBits(const std::string& destLine){
    uint16_t destBits = 0;

    for (char eachChar: destLine){
        switch(eachChar) {
            case 'A': 
                destBits |= 0x20;
                break;
            case 'D': 
                destBits |= 0x10;
                break;
            case 'M': 
                destBits |= 0x08;
                break;
        }
    }
    return destBits;
}

std::optional<std::string> parseJumpField(const std::string& line){
    auto jumpFieldBeginMarker = line.find(";");

    if (jumpFieldBeginMarker != std::string::npos){
        return line.substr(jumpFieldBeginMarker + 1);
    }
    else {
        return std::nullopt;
    }
}