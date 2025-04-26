#include <gtest/gtest.h>
#include "../include/GameState.h"
#include "../include/Board.h"
#include "../include/Tank.h"
#include "../include/Shell.h"
#include "../include/Wall.h"
#include "../include/Mine.h"

class GameStateTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Create a 10x10 board
        board = new Board(10, 10);

        // Place tanks on board
        tank1 = board->placeObject(std::make_unique<Tank>(Position(2, 2), 1));
        tank1 = board->placeObject(std::make_unique<Tank>(Position(7, 7), 2));

        // Create walls
        wall1 = board->placeObject(std::make_unique<Wall>(Position(4, 4)));
        wall2 = board->placeObject(std::make_unique<Wall>(Position(5, 5)));

        // Create mine
        mine = board->placeObject(std::make_unique<Mine>(Position(3, 6)));
    }

    void TearDown() override {
        delete board;
    }

    Board *board;
    Tank *tank1;
    Tank *tank2;
    Wall *wall1;
    Wall *wall2;
    Mine *mine;
    std::vector<Shell *> shells;
};

TEST_F(GameStateTest, ConstructorAndGetters) {
    GameState state(*board, 1);

    EXPECT_EQ(&state.getBoard(), board);
    EXPECT_EQ(state.getPlayerTank(), tank1);
    EXPECT_EQ(state.getEnemyTank(), tank2);
    // EXPECT_EQ(state.getShells().size(), 0);
}

TEST_F(GameStateTest, IsInLineOfSight_ClearPath) {
    // Position tank1 directly to the left of tank2 with no obstacles
    tank1->setDirection(Direction::RIGHT);
    board->moveObject(Position(2, 2), Position(2, 7));

    // Remove walls between them
    board->removeObject(wall1->getPosition());
    board->removeObject(wall2->getPosition());

    GameState state1(*board, 1);
    GameState state2(*board, 2);

    // Tank1 should be able to see tank2
    EXPECT_TRUE(state1.isInLineOfSight(tank2->getPosition()));

    // Tank2 is not facing tank1 by default
    tank2->setDirection(Direction::UP); // Ensure it's not facing tank1
    EXPECT_FALSE(state2.isInLineOfSight(tank1->getPosition()));

    // Change tank2's direction to face tank1
    tank2->setDirection(Direction::LEFT);
    EXPECT_TRUE(state2.isInLineOfSight(tank1->getPosition()));
}

TEST_F(GameStateTest, IsInLineOfSight_Blocked) {
    // Position tank1 directly to the left of tank2
    tank1->setDirection(Direction::RIGHT);
    board->moveObject(Position(2, 2), Position(2, 7));

    // Place a wall between them
    board->placeObject(std::make_unique<Wall>(Position(4, 7)));

    GameState state(*board, 1);

    // Wall should block line of sight
    EXPECT_FALSE(state.isInLineOfSight(tank2->getPosition()));
}

TEST_F(GameStateTest, IsShellApproaching) {
    // Create a shell heading toward tank1
    board->placeObject(std::make_unique<Shell>(Position(5, 2), Direction::LEFT, 2));

    GameState state1(*board, 1);
    GameState state2(*board, 2);

    // Shell is approaching tank1
    EXPECT_TRUE(state1.isShellApproaching());

    // Shell is not approaching tank2
    EXPECT_FALSE(state2.isShellApproaching());

    // Create a shell from tank1 - should not be considered as approaching tank1
    board->placeObject(std::make_unique<Shell>(Position(3, 2), Direction::RIGHT, 1));

    // Own shell should not be considered as approaching
    EXPECT_TRUE(state1.isShellApproaching()); // Only the enemy shell counts
}

TEST_F(GameStateTest, GetNearbyEmptyPositions) {
    GameState state(*board, 1);

    // Get empty positions around tank1
    std::vector<Position> emptyPositions = state.getNearbyEmptyPositions(tank1->getPosition());

    // Should have at least some empty positions nearby
    EXPECT_FALSE(emptyPositions.empty());

    // Verify that none of the positions have objects
    for (const Position &pos: emptyPositions) {
        EXPECT_FALSE(board->isOccupied(pos));
    }
}

TEST_F(GameStateTest, IsSafePosition) {
    GameState state(*board, 1);

    // Empty position should be safe
    EXPECT_TRUE(state.isSafePosition(Position(1, 1)));

    // Wall position should not be safe
    EXPECT_FALSE(state.isSafePosition(wall1->getPosition()));

    // Mine position should not be safe
    EXPECT_FALSE(state.isSafePosition(mine->getPosition()));

    // Tank position should not be safe
    EXPECT_FALSE(state.isSafePosition(tank1->getPosition()));
    EXPECT_FALSE(state.isSafePosition(tank2->getPosition()));
}
