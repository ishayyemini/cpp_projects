#ifndef BFS_H
#define BFS_H

#include <vector>
#include <string>
#include <queue>
#include <utility>
#include <board/direction.h>



namespace bfs_utils {

    struct Cell {
        std::pair<int, int> position;
        int parentDir;
    };

    int bfsToOpponent(
        const std::vector<std::vector<char>>& board,
        std::pair<int, int> tankPos,
        std::pair<int, int> targetPos
    );



}

#endif //BFS_H
