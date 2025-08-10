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

    // Log input errors
    void inputError(const std::string &message);

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
    static std::string getTimestamp();

    // File streams
    std::ofstream log_file;
    std::ofstream err_file;
    std::ofstream input_err_file;
};


#endif // LOGGER_H
