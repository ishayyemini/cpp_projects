#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include "../include/Action.h"
#include "../include/Direction.h"
#include "../include/SimpleAlgorithm.h"
#include "../include/PathfindingAlgorithm.h"
#include "../include/GameState.h"
#include "../include/Board.h"
#include "../include/Tank.h"
#include "../include/Shell.h"
#include "../include/Wall.h"
#include "../include/Mine.h"

class AlgorithmsTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Create a 10x10 board
        board = new Board(10, 10);

        // Create tanks
        tank1 = board->placeObject(std::make_unique<Tank>(Position(2, 2), 1));
        tank2 = board->placeObject(std::make_unique<Tank>(Position(7, 7), 2));

        // Create algorithms
        simpleAlgo = new SimpleAlgorithm();
        pathAlgo = new PathfindingAlgorithm();
    }

    void TearDown() override {
        delete board;

        // Delete algorithms
        delete simpleAlgo;
        delete pathAlgo;
    }

    // Helper to add a wall
    Wall *addWall(int x, int y) {
        return board->placeObject(std::make_unique<Wall>(Position(x, y)));
    }

    // Helper to add a mine
    Mine *addMine(int x, int y) {
        return board->placeObject(std::make_unique<Mine>(Position(x, y)));
    }

    // Helper to add a shell
    Shell *addShell(int x, int y, Direction::DirectionType dir, int ownerId) {
        return board->placeObject(std::make_unique<Shell>(Position(x, y), dir, ownerId));
    }

    Board *board;
    Tank *tank1;
    Tank *tank2;
    SimpleAlgorithm *simpleAlgo;
    PathfindingAlgorithm *pathAlgo;
};

TEST_F(AlgorithmsTest, SimpleAlgorithm_GetName) {
    EXPECT_EQ(simpleAlgo->getName(), "SimpleAlgorithm");
}

TEST_F(AlgorithmsTest, PathfindingAlgorithm_GetName) {
    EXPECT_EQ(pathAlgo->getName(), "PathfindingAlgorithm");
}

TEST_F(AlgorithmsTest, SimpleAlgorithm_ShootWhenInLineOfSight) {
    // Position tanks in line of sight
    tank1->setDirection(Direction::RIGHT);
    board->replaceObject(Position(2, 2), Position(2, 5));

    board->replaceObject(Position(7, 7), Position(7, 5));

    // Create game state from tank1's perspective
    GameState state(*board, 1);

    // Algorithm should decide to shoot when in line of sight
    Action action = simpleAlgo->decideAction(state);
    EXPECT_EQ(action, Action::SHOOT);
}

TEST_F(AlgorithmsTest, SimpleAlgorithm_EvadeApproachingShell) {
    // Add a shell approaching tank1
    addShell(5, 2, Direction::LEFT, 2);

    // Create game state from tank1's perspective
    GameState state(*board, 1);

    // Algorithm should decide to evade
    Action action = simpleAlgo->decideAction(state);
    EXPECT_NE(action, Action::NONE);
    EXPECT_NE(action, Action::SHOOT); // Shouldn't shoot when evading
}

// TEST_F(AlgorithmsTest, SimpleAlgorithm_RotateToFaceEnemy) {
//     // Tank is not in line of sight with enemy
//     tank1->setDirection(Direction::UP);
//
//     // Create game state from tank1's perspective
//     GameState state(*board, 1);
//
//     // Algorithm should decide to rotate to face enemy
//     Action action = simpleAlgo->decideAction(state);
//
//     // Should be some type of rotation
//     EXPECT_TRUE(action == Action::ROTATE_LEFT_EIGHTH ||
//         action == Action::ROTATE_RIGHT_EIGHTH ||
//         action == Action::ROTATE_LEFT_QUARTER ||
//         action == Action::ROTATE_RIGHT_QUARTER);
// }

TEST_F(AlgorithmsTest, PathfindingAlgorithm_FindPathToEnemy) {
    // Create a simple maze with walls
    addWall(3, 2);
    addWall(3, 3);
    addWall(3, 4);
    addWall(4, 4);
    addWall(5, 4);
    addWall(6, 4);

    // Create game state from tank1's perspective
    GameState state(*board, 1);

    // Algorithm should find a path around walls
    Action action = pathAlgo->decideAction(state);

    // Should either rotate or move forward to start navigating
    EXPECT_TRUE(action == Action::MOVE_FORWARD ||
        action == Action::ROTATE_LEFT_EIGHTH ||
        action == Action::ROTATE_RIGHT_EIGHTH ||
        action == Action::ROTATE_LEFT_QUARTER ||
        action == Action::ROTATE_RIGHT_QUARTER);
}

TEST_F(AlgorithmsTest, PathfindingAlgorithm_ShootWhenInLineOfSight) {
    // Position tanks in line of sight
    tank1->setDirection(Direction::RIGHT);
    board->replaceObject(Position(2, 2), Position(2, 5));

    board->replaceObject(Position(7, 7), Position(7, 5));

    // Create game state from tank1's perspective
    GameState state(*board, 1);

    // Algorithm should decide to shoot when in line of sight
    Action action = pathAlgo->decideAction(state);
    EXPECT_EQ(action, Action::SHOOT);
}

TEST_F(AlgorithmsTest, PathfindingAlgorithm_EvadeApproachingShell) {
    // Add a shell approaching tank1
    addShell(5, 2, Direction::LEFT, 2);

    // Create game state from tank1's perspective
    GameState state(*board, 1);

    // Algorithm should decide to evade
    Action action = pathAlgo->decideAction(state);
    EXPECT_NE(action, Action::NONE);
}

TEST_F(AlgorithmsTest, BothAlgorithms_NoneWhenNoTank) {
    // Test with destroyed tanks
    tank1->destroy();
    tank2->destroy();

    GameState state(*board, 1);

    // Both algorithms should return NONE for destroyed tanks
    EXPECT_EQ(simpleAlgo->decideAction(state), Action::NONE);
    EXPECT_EQ(pathAlgo->decideAction(state), Action::NONE);
}
