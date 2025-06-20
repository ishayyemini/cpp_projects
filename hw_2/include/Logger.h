#ifndef LOGGER_H
#define LOGGER_H

#include <fstream>
#include <map>
#include <string>
#include <vector>

#include "ActionRequest.h"

class Logger {
public:
    // Singleton instance getter
    static Logger &getInstance();

    // Log a message to the log file
    void log(const std::string &message);

    void logActions(std::vector<std::tuple<bool, ActionRequest, bool, bool> > actions);

    void logResult(const std::string &message);

    // Log an error to the error file
    void error(const std::string &message);

    // Log input errors
    void inputError(const std::string &message);

    // Initialize with custom file paths
    bool init(const std::string &path);

    // Close log files
    void close();

    // Delete copy and assignment to ensure singleton pattern
    Logger(const Logger &) = delete;

    Logger &operator=(const Logger &) = delete;

    // Delete move and assignment to ensure singleton pattern
    Logger(Logger &&) = delete;

    Logger &operator=(Logger &&) = delete;

private:
    // Private constructor for singleton pattern
    Logger();

    ~Logger();

    // Get timestamp for log entries
    std::string getTimestamp() const;

    // File streams
    std::ofstream out_file;
    std::ofstream log_file;
    std::ofstream err_file;
    std::string input_err_file_path;
    std::ofstream input_err_file;

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

#endif // LOGGER_H
