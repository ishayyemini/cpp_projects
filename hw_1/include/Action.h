#ifndef ACTION_H
#define ACTION_H
#include <list>
#include <map>
#include <string>

enum Action {
    NONE,
    MOVE_FORWARD,
    MOVE_BACKWARD,
    ROTATE_LEFT_EIGHTH,
    ROTATE_RIGHT_EIGHTH,
    ROTATE_LEFT_QUARTER,
    ROTATE_RIGHT_QUARTER,
    SHOOT,
};

inline std::map<Action, std::string> action_strings = {
    {NONE, "None"},
    {MOVE_FORWARD, "Move Forward"},
    {MOVE_BACKWARD, "Move Backward"},
    {ROTATE_LEFT_EIGHTH, "Rotate Left Eighth"},
    {ROTATE_RIGHT_EIGHTH, "Rotate Right Eighth"},
    {ROTATE_LEFT_QUARTER, "Rotate Left Quarter"},
    {ROTATE_RIGHT_QUARTER, "Rotate Right Quarter"},
    {SHOOT, "Shoot"},
};

#endif //ACTION_H
