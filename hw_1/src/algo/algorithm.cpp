#include "algorithm.h"
#include "board_elements/shell.h"

//todo: return shell position if there is a threatening one, or return list of all threatening shells
bool Algorithm::exists_threatening_shells(std::pair<int, int> tank_position) {
    for (auto i=0; i<board->getHeight(); i++) {
        for (auto j=0; j<board->getWidth(); j++) {
            BoardElement* element = board->getBoardElement(i, j);
            //check if there is a shell in cell i,j on the board
            //todo: implement  isShellDangerous so it checks if the shell can get tank if he doesn't move.
            if (dynamic_cast<Shell*>(element) && isShellDangerous(tank_position)) {
                return true;
            }
        }
    }
    return false;
}

bool Algorithm::isTankThreaten(std::pair<int, int> tank_position) {
    if (exists_threatening_shells(tank_position) || is_enemy_tank_threatening(tank_position)) {
        return true;
    }
    return false;
}


bool  Algorithm::isShellDangerous(std::pair<int, int> tank_position) {
    //todo: implement this
    return false;
}

bool Algorithm::isActionValid() {
    // TODO: Validate the last suggested action (use board state, position, etc.)
    return true;
}