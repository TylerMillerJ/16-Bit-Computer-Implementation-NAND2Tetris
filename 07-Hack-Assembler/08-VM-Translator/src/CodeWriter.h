#ifndef CODER_WRITER_H
#define CODER_WRITER_H

#include "Parser.h"

#include <fstream>
#include <string>


class CodeWriter
{
    public:
        /** Opens a .asm file with an opened writing stream*/
        exlicit CodeWriter(const std::string& filename);

        /** Writes the assembler code that implements the given command */
        void writeArithmatic(std::string command);

        /** Writes the output file in assembly for push pop */
        void writePushPop(CommandType commandType);

    private:
        std::ofstream outputFile;
};


#endif
