#include "command_line_argument_processing.h"

#include <iostream>
#include <cstdlib>
#include <fstream>
#include <string>


// The program keeps opens the .asm file and returns an opened .hack file

std::ofstream takeCommandLineArgumentAndReturnOpenHackFileStream(const char* arg) {
    std::string fileNameWithExtension = turnArgToString(arg);
    std::string filenameWithoutExtension = removeFileExtensionFromName(fileNameWithExtension);

    std::ofstream hackFileStream = createAndOpenHackFile(filenameWithoutExtension);

    std::size_t lastSlash = filenameWithoutExtension.find_last_of("/\\");
    std::string filenameOnly = filenameWithoutExtension.substr(lastSlash + 1);

    std::cout << "Created: " << filenameOnly << ".hack" << std::endl;

    return hackFileStream;
}


std::string turnArgToString(const char* arg){
    std::string argumentConvertedToString(arg);
    return argumentConvertedToString;
}

void checkThatTheFileIsASM(const std::string& fileNameWithExtension){
    std::string suffix = ".asm";
    if (fileNameWithExtension.size() < suffix.size() || fileNameWithExtension.substr(fileNameWithExtension.size() - 4) != suffix){
        std::cerr <<"Error: filetype is not asm; The input filetype must be an .asm file\n";
        std::exit(1);
    }
}

std::string removeFileExtensionFromName(const std::string& fileNameWithExtension) {
    checkThatTheFileIsASM(fileNameWithExtension);

    std::string fileNameWithoutExtension = fileNameWithExtension.substr(0,fileNameWithExtension.size() - 4);

    return fileNameWithoutExtension;
}

std::ofstream createAndOpenHackFile(const std::string& fileName) {
    std::size_t lastSlash = fileName.find_last_of("/\\");

    std::string filenameOnly = fileName.substr(lastSlash + 1);

    std::ofstream hackFileStream(filenameOnly + ".hack", std::ios::out | std::ios::trunc);

    return hackFileStream;
}

