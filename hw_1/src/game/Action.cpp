#include "Action.h"

#include <ostream>


inline std::ostream &operator<<(std::ostream &os, Action action) {
    switch (action) {
        case NONE:
            os << "None";
            break;
        case MOVE_FORWARD:
            os << "Move Forward";
            break;
        case MOVE_BACKWARD:
            os << "Move Backward";
            break;
        case ROTATE_LEFT_EIGHTH:
            os << "Rotate Left Eighth";
            break;
        case ROTATE_RIGHT_EIGHTH:
            os << "Rotate Right Eighth";
            break;
        case ROTATE_LEFT_QUARTER:
            os << "Rotate Left Quarter";
            break;
        case ROTATE_RIGHT_QUARTER:
            os << "Rotate Right Quarter";
            break;
        case SHOOT:
            os << "Shoot";
            break;
    }
    return os;
}
