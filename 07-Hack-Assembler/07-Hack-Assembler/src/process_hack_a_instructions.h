#ifndef PROCCESS_HACK_A_INSTRUCTIONS
#define PROCCESS_HACK_A_INSTRUCTIONS
 
#include <bit>
#include <string>
#include <fstream>
#include <cstdint>
#include <optional>
#include <iostream>
#include <charconv>

using std::uint16_t;


void proccessHackAInstructionSecondPass(std::string& line, std::ofstream& hackfile);

void proccessHackAInstrutionFirstPass(std::string& line);

void writeSymbolValueAsBinaryToHackFile(uint16_t& symbolTableValue, std::ofstream& hackfile);

bool isHackAInstruction(const std::string& line);

std::string removeHackADeclarationSymbol(const std::string& line);

std::optional<uint16_t> isWholeNumber(const std::string& line);


#endif