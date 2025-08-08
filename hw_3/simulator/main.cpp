#include <cstring>
#include <iostream>
#include <thread>

#include "Logger.h"

using namespace std::chrono_literals;

int main(const int argc, char *argv[]) {
    if (argc != 2 && (argc != 3 || strcmp(argv[1], "-g"))) {
        std::cerr << "Usage: " << argv[0] << " [-g] <game-file>" << std::endl;
        return EXIT_FAILURE;
    }
    //
    // const bool visual = argc == 3 && strcmp(argv[1], "-g") == 0;
    // const std::string path = argc == 2 ? argv[1] : argv[2];
    //
    // Logger::getInstance().init(path);
    //
    // const auto player_factory = MyPlayerFactory();
    // const auto tank_algorithm_factory = MyTankAlgorithmFactory();
    // GameManager game(player_factory, tank_algorithm_factory);
    // game.readBoard(path);
    // game.setVisual(visual);
    // game.enableGameStateExport("game_state.txt"); // Enable export to file
    // game.run();
    //
    return EXIT_SUCCESS;
}
