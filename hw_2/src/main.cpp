#include <GameManager.h>
#include <iostream>
#include <thread>

#include "Logger.h"
#include "MyGameManager.h"
#include "MyPlayerFactory.h"
#include "MyTankAlgorithmFactory.h"

using namespace std::chrono_literals;

int main(const int argc, char *argv[]) {
    if (argc != 2 && (argc != 3 || strcmp(argv[1], "-g"))) {
        std::cerr << "Usage: " << argv[0] << " [-g] <game-file>" << std::endl;
        return EXIT_FAILURE;
    }

    const bool visual = argc == 3 && strcmp(argv[1], "-g") == 0;
    const std::string path = argc == 2 ? argv[1] : argv[2];

    int last_dot = path.find_last_of(".");
    int last_slash = path.find_last_of("/");
    std::string name;
    if (last_dot > last_slash) name = path.substr(last_slash + 1, last_dot - last_slash - 1);
    else name = path.substr(last_slash + 1);

    Logger::getInstance().init("output_" + name + ".txt");

    MyGameManager game(MyPlayerFactory{}, MyTankAlgorithmFactory{});
    game.readBoard(path);
    game.setVisual(visual);
    game.run();

    return EXIT_SUCCESS;
}

