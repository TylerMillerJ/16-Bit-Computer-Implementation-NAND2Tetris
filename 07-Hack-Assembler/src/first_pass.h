#ifndef FIRST_PASS_H
#define FIRST_PASS_H

#include <cstdint>
#include <fstream>
#include <string>

using std::uint16_t;


void completeFirstPass(std::ifstream& asmFile);

std::string cleanLine(const std::string& line);

void proccessHackAInstrutionFirstPass(std::string& line);

#endif