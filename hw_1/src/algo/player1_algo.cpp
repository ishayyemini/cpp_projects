#include "player1_algo.h"
#include "game/action.h"
#include "algorithm.h"


Player1Algo::Player1Algo(GameBoard &board)
    : Algorithm(board), board_graph(board) {
}

Action Player1Algo::getNextAction() {
    const auto &player2_tank = board.getPlayerTank(2, 0);
    const auto &player1_tank = board.getPlayerTank(1, 1);

    if (!player2_tank || !player1_tank) {
        return NONE;
    }

    if (isTankThreaten(player1_tank->getPosition())) {
        const Action escape_action = escape();
        if (escape_action == NONE) {
            return suicide_mission(); //aka - try to get tie at least
        }
        return escape_action;
    }
    return chase();
}


Action Player1Algo::escape() {
    // TODO: Implement escape logic
    return NONE;
}

Action Player1Algo::chase() {
    // TODO: Implement chasing logic
    return Action();
}

Action Player1Algo::suicide_mission() {
    // TODO: Implement aggressive action when no better options are available
    return Action();
}

