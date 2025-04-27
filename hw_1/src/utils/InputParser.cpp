#include "InputParser.h"

#include <fstream>
#include <iostream>

#include "Board.h"
#include "Logger.h"
#include "../board_elements/BoardElementFactory.h"

bool InputParser::parseDimensions(std::ifstream &inFile) {
    if (!(inFile >> width >> height) || width <= 0 || height <= 0) {
        error_messages.push_back("Failed to extract board dimensions");
        return false;
    }
    std::string dummy;
    std::getline(inFile, dummy);
    return true;
}

// if there are more char we ignore them, but there needs to be at lead width * height
bool InputParser::populateBoard(std::ifstream &inFile) {
    std::string line;

    for (size_t row = 0; row < height; ++row) {
        if (!std::getline(inFile, line)) {
            error_messages.push_back("Map is shorter than specified height");
            line = " ";
        }

        for (size_t col = 0; col < width; ++col) {
            const char symbol = col < line.length() ? line[col] : ' ';
            if (symbol == '1' && getTank1() != nullptr) {
                error_messages.push_back("Multiple Tank 1");
                continue;
            }
            if (symbol == '2' && getTank2() != nullptr) {
                error_messages.push_back("Multiple Tank 1");
                continue;
            }
            if (symbol != '1' && symbol != '2' && symbol != '@' && symbol != '#' && symbol != ' ') {
                error_messages.push_back("Unknown symbol '" + std::string{symbol} + "'");
            }
            std::unique_ptr<GameObject> newElement = BoardElementFactory::create(symbol, Position(col, row));
            board->placeObject(std::move(newElement));
        }

        if (line.length() > width) {
            error_messages.push_back("Line " + std::to_string(row + 1) + " is longer than specified width");
        }
        if (line.length() < width) {
            error_messages.push_back("Line " + std::to_string(row + 1) + " is shorter than specified width");
        }
    }

    if (std::getline(inFile, line)) {
        error_messages.push_back("Map is longer than specified height");
    }

    return true;
}

Board *InputParser::parseInputFile(const std::string &file_name) {
    std::ifstream inFile(file_name);

    // check if file opened successfully
    if (!inFile) {
        std::cerr << "Error: Failed to create board. Could not open file " << file_name << " for reading.\n";
        error_messages.push_back("Error opening file " + file_name);
        return nullptr; // TODO maybe include a default map?
    }

    if (!parseDimensions(inFile)) {
        return nullptr;
    }

    Logger::getInstance().log(
        "Dimensions read from file " + file_name + ": " + std::to_string(width) + "x" + std::to_string(height));

    // create Board object of size width x height
    board = new Board(width, height);
    populateBoard(inFile);
    Logger::getInstance().log("Board loaded successfully");
    inFile.close();

    for (auto msg: error_messages) {
        Logger::getInstance().inputError(msg);
    }

    return board;
}

Tank *InputParser::getTank1() {
    if (board == nullptr) {
        return nullptr;
    }
    return board->getPlayerTank(1);
}

Tank *InputParser::getTank2() {
    if (board == nullptr) {
        return nullptr;
    }
    return board->getPlayerTank(2);
}


