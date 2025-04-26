#include "InputParser.h"

#include <fstream>
#include <iostream>

#include "Board.h"
#include "../board_elements/BoardElementFactory.h"

bool InputParser::parseDimensions(std::ifstream &inFile) {
    if (!(inFile >> width >> height) || width <= 0 || height <= 0) {
        error_messages.push_back("Error: Invalid board dimensions.\n");
        return false;
    }
    std::string dummy;
    std::getline(inFile, dummy);
    return true;
}

// if there are more char we ignore them, but there needs to be at lead width * height
bool InputParser::populateBoard(std::ifstream &inFile) {
    std::string line;
    board = new Board(width, height);

    for (int row = 0; row < height; ++row) {
        if (!std::getline(inFile, line)) {
            error_messages.push_back("Error: Missing row " + std::to_string(row + 1) + " in file.\n");
            line = " ";
        }

        for (int col = 0; col < width; ++col) {
            const char symbol = col < line.length() ? line[col] : ' ';
            std::unique_ptr<GameObject> newElement = BoardElementFactory::create(symbol, Position(row, col));
            board->placeObject(std::move(newElement));
        }
    }

    return true;
}

Board *InputParser::parseInputFile(const std::string &file_name) {
    std::ifstream inFile(file_name);

    // check if file opened successfully
    if (!inFile) {
        error_messages.push_back("Error opening file " + file_name);
        return nullptr; // TODO maybe include a default map?
    }

    if (!parseDimensions(inFile)) {
        return nullptr;
    }

    // create Board object of size width x height
    board = new Board(width, height);
    populateBoard(inFile);
    std::cout << "Board loaded from '" << file_name << "':\n";
    inFile.close();
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
    return board->getPlayerTank(1);
}


