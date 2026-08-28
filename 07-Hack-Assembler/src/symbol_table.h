#ifndef SYMBOL_TABLE_H
#define SYMBOL_TABLE_H

#define SCREEN_MEMORY 16384
#define KBD_MEMORY 24576

#include <unordered_map>
#include <string>
#include <cstdint>
#include <iostream>

using std::uint16_t;



//Global Data
extern std::unordered_map<std::string, std::uint16_t> symbolTable;

//Functions
uint16_t findSymbolTableValue(std::string& line);

bool addedLabelsToSymbolTable(const std::string& line, const uint16_t& instructionLineNumber);

bool isLabel(const std::string& line);


#endif