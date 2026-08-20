#ifndef COMMAND_LINE_ARGUMENT_PROCESSING_H
#define COMMAND_LINE_ARGUMENT_PROCESSING_H

#include <iostream>
#include <cstdlib>
#include <fstream>
#include <string>



std::ofstream takeCommandLineArgumentAndReturnOpenHackFileStream(const char* arg);

std::string turnArgToString(const char* arg);

void checkThatTheFileIsASM(const std::string& fileNameWithExtension);

std::string removeFileExtensionFromName(const std::string& fileNameWithExtension);

std::ofstream createAndOpenHackFile(const std::string& fileName);

#endif