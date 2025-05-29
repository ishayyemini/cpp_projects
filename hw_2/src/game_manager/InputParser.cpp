#include "InputParser.h"

#include <fstream>
#include <iostream>

#include "Board.h"
#include "Logger.h"
#include "GameObjectFactory.h"
#include "StringUtils.h"

void InputParser::addErrorMessage(const std::string &message) {
    error_messages.push_back(message);
}


bool InputParser::parseBoardConfig(std::ifstream &inFile,
                                   size_t &retrieved_data,
                                   const std::string &expected_field_name) {
    std::string line;
    if (!std::getline(inFile, line)) {
        addErrorMessage("Failed to extract line from file.");
        return false;
    }
    const char delimiter = '=';
    auto split_line = StringUtils::split_and_trim(line, delimiter);

    const bool format_valid = (split_line.size() == 2 && split_line[0] == expected_field_name);
    if (!format_valid) {
        addErrorMessage("Invalid format for \"" + expected_field_name + "\" in line: \"" + line + "\"");
        return false;
    }

    retrieved_data = std::stoi(split_line[1]); // Set to the extracted value
    return true;
}


void InputParser::populateBoard(std::ifstream &inFile) {
    std::string line;
    for (size_t row = 0; row < height; ++row) {
        if (!std::getline(inFile, line)) {
            addErrorMessage("Map is shorter than specified height");
            line = " ";
        }
        processLine(row, line);
        validateLineLength(row, line);
    }
    if (std::getline(inFile, line)) {
        addErrorMessage("Map is longer than specified height");
    }
}

void InputParser::processLine(size_t row, const std::string &line) {
    for (size_t col = 0; col < width; ++col) {
        char symbol = col < line.length() ? line[col] : ' ';

        if (!isValidSymbol(symbol)) {
            addErrorMessage("Unknown symbol '" + std::string{symbol} + "'");
            symbol = default_symbol;
        }

        auto obj = GameObjectFactory::create(symbol, Position(col, row));
        board->placeObject(std::move(obj));
    }
}

bool InputParser::isValidSymbol(char c) {
    return validSymbols.contains(c);
}

void InputParser::validateLineLength(size_t row, const std::string &line) {
    if (line.length() > width) {
        addErrorMessage("Line " + std::to_string(row + 1) + " is longer than specified width");
    } else if (line.length() < width) {
        addErrorMessage("Line " + std::to_string(row + 1) + " is shorter than specified width");
    }
}

bool InputParser::parseBoardInfo(std::ifstream &inFile) {
    std::string board_info;

    if (!std::getline(inFile, board_description)) {
        addErrorMessage("Failed to extract board info (board title + board description).");
        return false;
    }

    board_info += "Board description: " + board_description;

    auto checkParse = [this, &inFile, &board_info](size_t &field, const std::string &field_name) {
        if (!parseBoardConfig(inFile, field, field_name)) {
            addErrorMessage("Failed to extract " + field_name + " from file.");
            return false;
        }
        board_info += ", " + field_name + ": " + std::to_string(field);
        return true;
    };
    if (!checkParse(max_steps, "MaxSteps") ||
        !checkParse(num_shells, "NumShells") ||
        !checkParse(height, "Rows") ||
        !checkParse(width, "Cols")) {
        return false;
    }
    Logger::getInstance().log("Board Info Read: " + board_info);
    return true;
}

void InputParser::addErrorMessagesToLog() {
    for (const std::string &msg: error_messages) {
        Logger::getInstance().inputError(msg);
    }
}

std::unique_ptr<Board> InputParser::parseInputFile(const std::string &file_name) {
    Logger::getInstance().log("Parsing file:  " + file_name);
    std::ifstream inFile(file_name);
    if (!inFile) {
        std::cerr << "Error: Failed to create board. Could not open file " << file_name << " for reading.\n";
        addErrorMessage("Error opening file " + file_name);
        addErrorMessagesToLog();
        return nullptr;
    }
    if (!parseBoardInfo(inFile)) {
        addErrorMessagesToLog();
        return nullptr;
    }

    board = std::make_unique<Board>(board_description, max_steps, num_shells, width, height);
    populateBoard(inFile);
    Logger::getInstance().log("Board loaded successfully");
    inFile.close();

    addErrorMessagesToLog();
    return std::move(board);
}




