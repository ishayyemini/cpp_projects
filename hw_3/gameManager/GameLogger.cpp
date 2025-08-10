#include "GameLogger.h"

#include <iostream>
#include <chrono>
#include <vector>

using namespace GameManager_322868852_340849710;

bool GameLogger::init(const std::string &map_name) {
    const std::string out_file_path = "output_" + map_name + ".txt";

    if (initialized) {
        close();
    }

    out_file.open(out_file_path, std::ios::out);
    if (!out_file.is_open()) {
        std::cerr << "Failed to open out file: " << out_file_path << std::endl;
        return false;
    }

    initialized = true;
    return true;
}

void GameLogger::close() {
    if (out_file.is_open()) {
        out_file.close();
    }

    initialized = false;
}

void GameLogger::logActions(std::vector<std::tuple<bool, ActionRequest, bool, bool> > actions) {
    if (!initialized) {
        std::cerr << "Logger not initialized" << std::endl;
        return;
    }

    size_t i = 0;
    for (const auto &[gone, action, result, killed]: actions) {
        if (gone) {
            out_file << "killed";
        } else {
            out_file << action_strings[action];
            if (!result) out_file << " (ignored)";
            if (killed) out_file << " (killed)";
        }
        if (i++ < actions.size() - 1) out_file << ", ";
        else out_file << std::endl;
    }

    out_file.flush();
}

void GameLogger::logResult(const std::string &message) {
    if (!initialized) {
        std::cerr << "Logger not initialized" << std::endl;
        return;
    }

    out_file << message << std::endl;
    out_file.flush();
}
