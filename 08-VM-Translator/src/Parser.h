#ifndef PARSER_H
#define PARSER_H

#include <fstream>
#include <string>

enum class CommandType
{
    C_ARITHMETIC, 
    C_PUSH,
    C_POP,
    C_LABEL,
    C_GOTO,
    C_IF,
    C_FUNCTION,
    C_RETURN,
    C_CALL
};


class Parser
{
    public:
        /** Opens the specified file as an object */
        explicit Parser(const std::string& vmFilename);

        /** Returns True if there are  more commands to process*/
        bool hasMoreCommands();

        /** Reads the next command and makes it current command */
        void advance();

        /** Returns the type of the current command */
        CommandType commandType();

    private:
        std::ifstream vmFile;
};

#endif


/*
API: 
    Opens File
    Reads A VM Command
    Parses into Lexical components, Saving for convenient Access
    Ignores Comments and Whitespace
*/