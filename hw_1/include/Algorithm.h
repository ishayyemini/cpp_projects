#ifndef ALGORITHM_H
#define ALGORITHM_H

#include <string>

#include "Action.h"
#include "GameState.h"

class Algorithm {
public:
    Algorithm() = default;

    virtual std::string getName() const { return "Algorithm"; };

    virtual Action decideAction(const GameState &state) const { return NONE; }

    virtual ~Algorithm() = default;

protected:
    Action handleImmediateThreat(const GameState &state, int tankDistance) const;

    Action escape(const GameState &state, bool can_change_direction, int available_steps = 1,
                  Action default_action = NONE) const;
};

#endif //ALGORITHM_H
