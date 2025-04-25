#include <iostream>

#include "game/game_manager.h"
#include "utils/file_utils.h"


int main() {
    //todo: change it so we get it from user
    const std::string path = "../input/map.txt";

    // if (const int status = file_utils::loadBoard(gb, path); status == 0) {
    //     std::cout << "Map loaded successfully." << std::endl;
    // } else {
    //     std::cerr << "Failed to load map." << std::endl;
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

