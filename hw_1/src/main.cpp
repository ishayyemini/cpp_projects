#include <GameManager.h>
#include <iostream>
#include <thread>

#include "Logger.h"
#include "InputParser.h"

using namespace std::chrono_literals;

int main(const int argc, char *argv[]) {
    if (argc != 2 && (argc != 3 || strcmp(argv[1], "-g"))) {
        std::cerr << "Usage: " << argv[0] << " [-g] <game-file>" << std::endl;
        return EXIT_FAILURE;
    }

    const bool visual = argc == 3 && strcmp(argv[1], "-g") == 0;
    const std::string path = argc == 2 ? argv[1] : argv[2];

    Logger::getInstance().init(
        "output_" + path.substr(path.find_last_of("/\\") + 1, path.find_last_of(".")) + ".txt"
    );

    Board *board = InputParser().parseInputFile(path);

    if (board == nullptr) {
        std::cerr << "Can't open file " << path << std::endl;
        return EXIT_FAILURE;
    }

    GameManager game_manager(*board, visual);
    while (!game_manager.isGameOver()) {
        game_manager.processStep();
        if (visual) std::this_thread::sleep_for(200ms);
    }

    Logger::getInstance().log(game_manager.getGameResult());
    if (visual) {
        std::cout << game_manager.getGameResult() << std::endl;
    }

    delete board;

    return EXIT_SUCCESS;
}

