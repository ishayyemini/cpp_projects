#include <cstring>
#include <iostream>
#include <thread>
#include <dlfcn.h>

#include "AlgorithmRegistrar.h"
#include "GameManagerRegistrar.h"
#include "InputParser.h"
#include "Logger.h"

using namespace std::chrono_literals;
using namespace UserCommon_322868852_340849710;

int main(const int argc, char *argv[]) {
    if (argc != 2 && (argc != 3 || strcmp(argv[1], "-g"))) {
        std::cerr << "Usage: " << argv[0] << " [-g] <game-file>" << std::endl;
        return EXIT_FAILURE;
    }

    const std::string path = argc == 2 ? argv[1] : argv[2];

    auto &alg_reg = AlgorithmRegistrar::getAlgorithmRegistrar();
    alg_reg.createAlgorithmFactoryEntry("Algorithm_322868852_340849710");
    void *alg_dl = dlopen("../algorithm/Algorithm_322868852_340849710.so", RTLD_LAZY | RTLD_GLOBAL);
    if (!alg_dl) {
        std::cerr << "Failed to load library: " << dlerror() << std::endl;
        return EXIT_FAILURE;
    }

    auto &gm_reg = GameManagerRegistrar::getGameManagerRegistrar();
    gm_reg.createGameManagerFactoryEntry("GameManager_322868852_340849710");
    void *gm_dl = dlopen("../gameManager/GameManager_322868852_340849710.so", RTLD_LAZY | RTLD_GLOBAL);
    if (!gm_dl) {
        std::cerr << "Failed to load library: " << dlerror() << std::endl;
        return EXIT_FAILURE;
    }

    Logger::getInstance().init(path);

    auto input_parser = InputParser();
    input_parser.parseInputFile(path);

    std::unique_ptr<AbstractGameManager> game;
    for (const auto &gameManager: gm_reg) {
        game = gameManager.createGameManager(false);
    }

    std::unique_ptr<Player> player1;
    std::unique_ptr<Player> player2;
    TankAlgorithmFactory tank_factory1;
    TankAlgorithmFactory tank_factory2;
    for (const auto &alg: alg_reg) {
        player1 = alg.createPlayer(1, input_parser.getWidth(), input_parser.getHeight(), input_parser.getMaxSteps(),
                                   input_parser.getNumShells());
        player2 = alg.createPlayer(2, input_parser.getWidth(), input_parser.getHeight(), input_parser.getMaxSteps(),
                                   input_parser.getNumShells());
        tank_factory1 = alg.getTankAlgorithmFactory();
        tank_factory2 = alg.getTankAlgorithmFactory();
    }

    game->run(input_parser.getWidth(), input_parser.getHeight(), *input_parser.getSatelliteView(),
              input_parser.getBoardDescription(), input_parser.getMaxSteps(), input_parser.getNumShells(), *player1,
              "player1", *player2, "player2", tank_factory1, tank_factory2);

    AlgorithmRegistrar::getAlgorithmRegistrar().clear();
    GameManagerRegistrar::getGameManagerRegistrar().clear();

    dlclose(alg_dl);
    dlclose(gm_dl);

    return EXIT_SUCCESS;
}
