#include "ActionRequest.h"

#include <ostream>

namespace GameManager_322868852_340849710 {
    inline std::ostream &operator<<(std::ostream &os, ActionRequest action) {
        switch (action) {
            case ActionRequest::DoNothing:
                os << "None";
                break;
            case ActionRequest::MoveForward:
                os << "Move Forward";
                break;
            case ActionRequest::MoveBackward:
                os << "Move Backward";
                break;
            case ActionRequest::RotateLeft45:
                os << "Rotate Left Eighth";
                break;
            case ActionRequest::RotateRight45:
                os << "Rotate Right Eighth";
                break;
            case ActionRequest::RotateLeft90:
                os << "Rotate Left Quarter";
                break;
            case ActionRequest::RotateRight90:
                os << "Rotate Right Quarter";
                break;
            case ActionRequest::Shoot:
                os << "Shoot";
                break;
            case ActionRequest::GetBattleInfo:
                os << "Get Battle Info";
                break;
        }
        return os;
    }
}
