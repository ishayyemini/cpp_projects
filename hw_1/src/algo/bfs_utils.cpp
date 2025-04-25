#include "bfs_utils.h"

#include "board/game_board.h"

namespace bfs_utils {
    //todo: why passing the tank?
    int bfsToOpponent(const GameBoard& board,
                      std::pair<int, int> tankPos,
                      std::pair<int, int> targetPos) {
        int rows = board.getHeight();
        int cols = board.getWidth();

        //init matrix that keep track of which node we already visited - init to false
        std::vector visited(rows, std::vector(cols, false));
        std::queue<Cell> q;

        q.push({tankPos, -1});
        visited[tankPos.first][tankPos.second] = true;

        while (!q.empty()) {
            Cell current_cell = q.front(); q.pop();

            if (current_cell.position == targetPos) {
                return current_cell.parentDir;
            }

            for (int d = 0; d < Direction::getDirectionSize(); d++) {\
                std::pair<int, int> direction = Direction::getOffset(d);
                int nx = current_cell.position.first + direction.first;
                int ny = current_cell.position.second + direction.second;

                //todo: check boundaries + fix using tunnel
                if (visited[nx][ny]) {
                    continue;
                }
                //didn't visited it yet
                BoardElement element = board.getBoardElement(nx, ny);
                if (cell == '#' || cell == '@') continue;

                visited[nx][ny] = true;
                int dirToPropagate = (current.parentDir == -1) ? d : current.parentDir;
                q.push({nx, ny, dirToPropagate});
            }
        }

        return -1;
    }
}
