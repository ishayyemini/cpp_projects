#ifndef LOGGER_H
#define LOGGER_H

#include <fstream>
#include <map>
#include <string>
#include <vector>

#include "ActionRequest.h"

namespace GameManager_322868852_340849710 {
    class GameLogger {
    public:
        GameLogger() : initialized(false) {
        }

        void logActions(std::vector<std::tuple<bool, ActionRequest, bool, bool> > actions);

        void logResult(const std::string &message);

        bool init(const std::string &map_name, const std::string &name1, const std::string &name2);

        void close();

        ~GameLogger() {
            close();
        }

        // Copy constructor
        GameLogger(const GameLogger &) : initialized(false) {
            // Cannot copy file streams, so we don't copy the state
            // User would need to call init() again on the copied object
        }

        // Copy assignment operator
        GameLogger &operator=(const GameLogger &other) {
            if (this != &other) {
                close(); // Close current file if open
                initialized = false;
                // Cannot copy file streams, so we don't copy the state
                // User would need to call init() again on the assigned object
            }
            return *this;
        }

        // Move constructor
        GameLogger(GameLogger &&other) noexcept : out_file(std::move(other.out_file)),
                                                  initialized(other.initialized) {
            other.initialized = false;
        }

        // Move assignment operator
        GameLogger &operator=(GameLogger &&other) noexcept {
            if (this != &other) {
                close(); // Close current file if open
                out_file = std::move(other.out_file);
                initialized = other.initialized;
                other.initialized = false;
            }
            return *this;
        }

    private:
        // File stream
        std::ofstream out_file;

        // Initialization status
        bool initialized;
    };


    inline std::map<ActionRequest, std::string> action_strings = {
        {ActionRequest::DoNothing, "None"},
        {ActionRequest::MoveForward, "Move Forward"},
        {ActionRequest::MoveBackward, "Move Backward"},
        {ActionRequest::RotateLeft45, "Rotate Left Eighth"},
        {ActionRequest::RotateRight45, "Rotate Right Eighth"},
        {ActionRequest::RotateLeft90, "Rotate Left Quarter"},
        {ActionRequest::RotateRight90, "Rotate Right Quarter"},
        {ActionRequest::Shoot, "Shoot"},
        {ActionRequest::GetBattleInfo, "Get Battle Info"},
    };
}

#endif // LOGGER_H
