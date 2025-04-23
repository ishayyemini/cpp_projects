//
// Created by rachel on 4/2/2025.
//

#ifndef SHELL_H
#define SHELL_H

class Shell final : public BoardElement {
    std::pair<int, int> position;

public:
    explicit Shell(const std::pair<int, int> &position): position(position) {
    }

    [[nodiscard]] std::string getSymbol() const override { return "☄️"; }
};

#endif //SHELL_H
