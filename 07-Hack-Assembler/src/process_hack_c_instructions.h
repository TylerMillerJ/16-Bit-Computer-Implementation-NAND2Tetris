#ifndef PROCCESS_HACK_C_INSTRUCTIONS
#define PROCCESS_HACK_C_INSTRUCTIONS

#include <cstdint>
#include <string>
#include <unordered_map>
#include <optional>
#include <fstream>
#include <bit>
#include <charconv>


using std::uint16_t;

void proccessHackCInstruction(std::string& line, std::ofstream& hackfile);

uint16_t buildBinaryOutput(const std::string& line);

void writeBinaryToFile(uint16_t& binaryCInstruction, std::ofstream& hackfile);

std::string parseCompField(std::string line);

std::optional<std::string> parseDestField(const std::string& line);

uint16_t setDestBits(const std::string& destLine);


std::optional<std::string> parseJumpField(const std::string& line);

#endif