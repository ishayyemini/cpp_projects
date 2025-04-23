#include "game_manager.h"

#include "board/position.h"


Winner GameManager::start_game() {
    while (!game_over) {
        game_step++;
        //perform player1_algo move

    }
    return winner;
}

bool GameManager::moveForward(Tank tank, Position position) {
    //we assume the algo_will give legal moves
    Direction direction = tank.getCannonDirection();


}
