#include <GameManager.h>
#include <iostream>

#include "game/game_manager.h"
#include "utils/file_utils.h"


int main(const int argc, char *argv[]) {
    if (argc != 2) {
        std::cerr << "Usage: " << argv[0] << " <game-file>" << std::endl;
    }

    const std::string path = argv[1];
    GameManager gm(path);
    const Winner winner = gm.startGame();

    switch (winner) {
        case TIE:
            std::cout << "Tie" << std::endl;
            break;
        case PLAYER_1:
            std::cout << "Player 1 Won!" << std::endl;
            break;
        case PLAYER_2:
            std::cout << "Player 2 Won!" << std::endl;
            break;
        case NO_WINNER:
            std::cout << "No winner" << std::endl;
            break;
    }
}

