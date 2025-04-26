#ifndef BFS_UTILS_H
#define BFS_UTILS_H
#include <Direction.h>
#include <GameState.h>
#include <unordered_map>

#include "TankState.h"


class BfsUtils {
public:
    Position execute_stateful_bfs(const GameState &state, std::unordered_map<TankState, TankState> &bfs_tree) const;

private:
    void BfsUtils::generateNextStates(
        const GameState &state,
        const TankState &current,
        int cost,
        std::queue<std::pair<TankState, int> > &q,
        std::unordered_set<TankState> &visited,
        std::unordered_map<TankState, TankState> &bfs_tree
    ) const;
};


#endif //BFS_UTILS_H
