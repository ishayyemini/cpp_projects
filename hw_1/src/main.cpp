#include <iostream>

#include "../include/GameManager.h"
#include "../include/InputParser.h"


int main(const int argc, char *argv[]) {
    if (argc != 2) {
        std::cerr << "Usage: " << argv[0] << " <game-file>" << std::endl;
    }

    const std::string path = argv[1];
    Board *board = InputParser().parseInputFile(path);

    GameManager game_manager(*board);
    while (!game_manager.isGameOver()) {
        game_manager.processStep();
    }

    std::cout << game_manager.getGameResult() << std::endl;

    delete board;
}

