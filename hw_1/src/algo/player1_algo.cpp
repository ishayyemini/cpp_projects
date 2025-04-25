#include "player1_algo.h"
#include "game/action.h"
#include "algorithm.h"


Player1Algo::Player1Algo(GameBoard *board)
    : Algorithm(board), board_graph(*board) {

}

Action Player1Algo::getNextAction(std::pair<int, int> tank_position) {
    if (isTankThreaten(tank_position)) {
        Action escape_action = escape();
        if (escape_action == Action::NONE) {
            return suicide_mission(); //aka - try to get tie at least
        }
        return escape_action;
    }
    return chase();
}


Action Player1Algo::escape() {
    // TODO: Implement escape logic
    return Action::NONE;
}

Action Player1Algo::chase() {
    // TODO: Implement chasing logic
    return Action();
}

Action Player1Algo::suicide_mission() {
    // TODO: Implement aggressive action when no better options are available
    return Action();
}

