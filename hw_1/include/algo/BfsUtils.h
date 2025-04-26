#ifndef BFS_UTILS_H
#define BFS_UTILS_H
#include <Direction.h>
#include <GameState.h>
#include <unordered_map>
#include <queue>
#include "TankState.h"


class BfsUtils {
public:
    static Position execute_stateful_bfs(const GameState &state, std::unordered_map<TankState, TankState> &bfs_tree);

private:
    static void generateNextStates(
        const GameState &state,
        const TankState &current,
        int cost,
        std::queue<std::pair<TankState, int> > &q,
        std::unordered_set<TankState> &visited,
        std::unordered_map<TankState, TankState> &bfs_tree
    );


    static void tryMove(const GameState &state, std::queue<std::pair<TankState, int>> &q, std::unordered_set<TankState> &visited,
    std::unordered_map<TankState, TankState> &bfs_tree,
    const TankState &fromState,
    const TankState &toState,
    int newCost
);

    static Position reconstructFirstMove(
        const std::unordered_map<TankState, TankState> &bfs_tree,
        const TankState &goalReached,
        const TankState &startState
    ) ;
};


#endif //BFS_UTILS_H
