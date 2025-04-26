#include <GameManager.h>
#include <iostream>
#include <thread>

#include "../include/GameManager.h"
#include "../include/InputParser.h"

using namespace std::chrono_literals;

int main(const int argc, char *argv[]) {
    if (argc != 2) {
        std::cerr << "Usage: " << argv[0] << " <game-file>" << std::endl;
        return EXIT_FAILURE;
    }

    const std::string path = argv[1];
    Board *board = InputParser().parseInputFile(path);

    if (board == nullptr) {
        std::cerr << "Can't open file " << path << std::endl;
        return EXIT_FAILURE;
    }

    std::cout << "managed to create board" << std::endl;

    GameManager game_manager(*board);
    while (!game_manager.isGameOver()) {
        game_manager.processStep();
        std::this_thread::sleep_for(500ms);
    }

    std::cout << game_manager.getGameResult() << std::endl;

    delete board;

    return EXIT_SUCCESS;
}

