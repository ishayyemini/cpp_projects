#ifndef INPUTPARSER_H
#define INPUTPARSER_H

#include "Board.h"

class InputParser {
    std::vector<std::string> error_messages;
    size_t width;
    size_t height;
    std::unique_ptr<Board> board;

    bool parseDimensions(std::ifstream &inFile);

    bool populateBoard(std::ifstream &inFile);

public:
    InputParser() = default;

    std::unique_ptr<Board> parseInputFile(const std::string &file_name);

    Tank *getTank1();

    Tank *getTank2();

    bool hasErrors() const { return !error_messages.empty(); }

    const std::vector<std::string> &getErrorMessages() const { return error_messages; }

    ~InputParser() = default;
};

#endif //INPUTPARSER_H
