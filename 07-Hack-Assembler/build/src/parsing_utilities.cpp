#include "parsing_utilities.h"

#include <string>

std::string cleanLine(const std::string& line){ //removing all whitespace and comments
    auto end = line.find_first_of(" \t/");
    if (end == std::string::npos){
        return line;
    }
    return line.substr(0, end);
}

