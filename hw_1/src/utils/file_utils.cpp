#include <fstream>
#include <iostream>

#include "board/game_board.h"
#include "file_utils.h"


//define helper functions in unnamed namespace
namespace {
    bool parseDimensions(std::ifstream &inFile, int &width, int &height);

    bool populateBoard(std::ifstream &inFile, GameBoard &board, int width, int height);
}


int file_utils::loadBoard(const std::string &fileName) {
    std::ifstream inFile(fileName); // open file for reading

    // check if file opened successfully
    if (!inFile) {
        std::cerr << "Error opening file " << fileName << std::endl;
        return -1;
    }

    int width{0}, height{0};

    if (!parseDimensions(inFile, width, height)) {
        return -1;
    }

    //create GameBoard object of size width x height
    GameBoard board(width, height);
    if (!populateBoard(inFile, board, width, height)) {
        return -1;
    }
    std::cout << "Board loaded from '" << fileName << "':\n";
    board.displayBoard();
    inFile.close();
    return 0;
}

namespace {
    bool parseDimensions(std::ifstream &inFile, int &width, int &height) {
        if (!(inFile >> width >> height) || width <= 0 || height <= 0) {
            std::cerr << "Error: Invalid board dimensions.\n";
            return false;
        }
        std::string dummy;
        std::getline(inFile, dummy);
        return true;
    }

    // if there are more char we ignore them, but there needs to be at lead width * height
    bool populateBoard(std::ifstream &inFile, GameBoard &board, int width, int height) {
        std::string line;

        for (int row = 0; row < height; ++row) {
            if (!std::getline(inFile, line)) {
                std::cerr << "Error: Missing row " << row + 1 << " in file.\n";
                //todo: maybe is possible to adjust this by adding additional empty rows
                return false;
            }

            // if (static_cast<int>(line.length()) < width) {
            //     std::cerr << "Error: Line " << row + 1 << " is too short (expected " << width << ", got " << line.
            //             length() << ").\n";
            //     //todo: maybe is possible to adjust this by adding additional empty rows
            //     return false;
            // }

            for (int col = 0; col < width; ++col) {
                char symbol = col < line.length() ? line[col] : ' ';
                //todo: this should never happen, maybe we can delete this validation
                if (!board.updateBoardElement(row, col, symbol)) {
                    return false;;
                }
            }
        }

        return true;
    }
}
