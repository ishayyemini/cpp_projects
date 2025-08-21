#include "InputParser.h"

#include <fstream>
#include <iostream>

#include "Logger.h"
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

void InputParser::populateBoard(std::ifstream &in_file) {
    std::string line;
    for (size_t row = 0; row < height; ++row) {
        if (!std::getline(in_file, line)) {
            addErrorMessage("Map is shorter than specified height");
            line = " ";
        }
        processLine(row, line);
        validateLineLength(row, line);
    }
    if (std::getline(in_file, line)) {
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

        satellite_view->setObjectAt(col, row, symbol);
    }
}

bool InputParser::isValidSymbol(char c) {
    return valid_symbols.contains(c);
}

void InputParser::validateLineLength(size_t row, const std::string &line) {
    if (line.length() > width) {
        addErrorMessage("Line " + std::to_string(row + 1) + " is longer than specified width");
    } else if (line.length() < width) {
        addErrorMessage("Line " + std::to_string(row + 1) + " is shorter than specified width");
    }
}

bool InputParser::parseBoardInfo(std::ifstream &in_file) {
    std::string board_info;

    if (!std::getline(in_file, board_description)) {
        addErrorMessage("Failed to extract board info (board title + board description).");
        return false;
    }

    board_info += "Board description: " + board_description;

    auto checkParse = [this, &in_file, &board_info](size_t &field, const std::string &field_name) {
        if (!parseBoardConfig(in_file, field, field_name)) {
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

void InputParser::parseInputFile(const std::string &file_name) {
    Logger::getInstance().log("Parsing file:  " + file_name);
    std::ifstream inFile(file_name);
    if (!inFile) {
        std::cerr << "Error: Failed to create board. Could not open file " << file_name << " for reading.\n";
        addErrorMessage("Error opening file " + file_name);
        addErrorMessagesToLog();
        return;
    }
    if (!parseBoardInfo(inFile)) {
        addErrorMessagesToLog();
        return;
    }

    satellite_view = std::make_unique<MySatelliteView>(width, height);
    populateBoard(inFile);
    Logger::getInstance().log("Board loaded successfully");
    inFile.close();

    addErrorMessagesToLog();
}

std::unique_ptr<SatelliteView> InputParser::getSatelliteView() {
    auto sw = std::make_unique<MySatelliteView>(width, height);
    for (size_t x = 0; x < width; ++x) {
        for (size_t y = 0; y < height; ++y) {
            sw->setObjectAt(x, y, satellite_view->getObjectAt(x, y));
        }
    }
    return sw;
}
