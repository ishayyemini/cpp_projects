#ifndef LOGGER_H
#define LOGGER_H

#include <fstream>
#include <string>

class Logger {
public:
    // Singleton instance getter
    static Logger &getInstance();

    // Log a message to the log file
    void log(const std::string &message);

    // Log an error to the error file
    void error(const std::string &message);

    // Initialize with custom file paths
    bool init(const std::string &logFilePath = "game.log",
              const std::string &errFilePath = "error.log");

    // Close log files
    void close();

    // Delete copy and assignment to ensure singleton pattern
    Logger(const Logger &) = delete;

    Logger &operator=(const Logger &) = delete;

private:
    // Private constructor for singleton pattern
    Logger();

    ~Logger();

    // Get timestamp for log entries
    std::string getTimestamp() const;

    // File streams
    std::ofstream log_file;
    std::ofstream err_file;

    // Initialization status
    bool initialized;
};

#endif // LOGGER_H
