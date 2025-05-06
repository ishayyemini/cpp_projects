#include <gtest/gtest.h>

#include "GameObject.h"

TEST(PositionTest, ConstructorDefault) {
    Position pos;
    EXPECT_EQ(pos.x, 0);
    EXPECT_EQ(pos.y, 0);
}

TEST(PositionTest, ConstructorWithValues) {
    Position pos(3, 5);
    EXPECT_EQ(pos.x, 3);
    EXPECT_EQ(pos.y, 5);
}

TEST(PositionTest, EqualityOperator) {
    Position pos1(2, 3);
    Position pos2(2, 3);
    Position pos3(4, 5);

    EXPECT_TRUE(pos1 == pos2);
    EXPECT_FALSE(pos1 == pos3);
}

TEST(PositionTest, InequalityOperator) {
    Position pos1(2, 3);
    Position pos2(2, 3);
    Position pos3(4, 5);

    EXPECT_FALSE(pos1 != pos2);
    EXPECT_TRUE(pos1 != pos3);
}

TEST(DirectionTest, GetDirectionDelta) {
    // Test each of the 8 directions
    Position upDelta = Direction::getDirectionDelta(Direction::UP);
    EXPECT_EQ(upDelta.x, 0);
    EXPECT_EQ(upDelta.y, -1);

    Position upRightDelta = Direction::getDirectionDelta(Direction::UP_RIGHT);
    EXPECT_EQ(upRightDelta.x, 1);
    EXPECT_EQ(upRightDelta.y, -1);

    Position rightDelta = Direction::getDirectionDelta(Direction::RIGHT);
    EXPECT_EQ(rightDelta.x, 1);
    EXPECT_EQ(rightDelta.y, 0);

    Position downRightDelta = Direction::getDirectionDelta(Direction::DOWN_RIGHT);
    EXPECT_EQ(downRightDelta.x, 1);
    EXPECT_EQ(downRightDelta.y, 1);

    Position downDelta = Direction::getDirectionDelta(Direction::DOWN);
    EXPECT_EQ(downDelta.x, 0);
    EXPECT_EQ(downDelta.y, 1);

    Position downLeftDelta = Direction::getDirectionDelta(Direction::DOWN_LEFT);
    EXPECT_EQ(downLeftDelta.x, -1);
    EXPECT_EQ(downLeftDelta.y, 1);

    Position leftDelta = Direction::getDirectionDelta(Direction::LEFT);
    EXPECT_EQ(leftDelta.x, -1);
    EXPECT_EQ(leftDelta.y, 0);

    Position upLeftDelta = Direction::getDirectionDelta(Direction::UP_LEFT);
    EXPECT_EQ(upLeftDelta.x, -1);
    EXPECT_EQ(upLeftDelta.y, -1);
}

TEST(DirectionTest, RotateDirection) {
    // Test 1/8 clockwise rotation
    Direction::DirectionType dir = Direction::UP;
    Direction::DirectionType rotated = Direction::rotateDirection(dir, true, false);
    EXPECT_EQ(rotated, Direction::UP_RIGHT);

    // Test 1/4 clockwise rotation
    rotated = Direction::rotateDirection(dir, true, true);
    EXPECT_EQ(rotated, Direction::RIGHT);

    // Test 1/8 counter-clockwise rotation
    dir = Direction::RIGHT;
    rotated = Direction::rotateDirection(dir, false, false);
    EXPECT_EQ(rotated, Direction::UP_RIGHT);

    // Test 1/4 counter-clockwise rotation
    rotated = Direction::rotateDirection(dir, false, true);
    EXPECT_EQ(rotated, Direction::UP);

    // Test wrapping clockwise
    dir = Direction::UP_LEFT;
    rotated = Direction::rotateDirection(dir, true, false);
    EXPECT_EQ(rotated, Direction::UP);

    // Test wrapping counter-clockwise
    dir = Direction::UP;
    rotated = Direction::rotateDirection(dir, false, false);
    EXPECT_EQ(rotated, Direction::UP_LEFT);
}
