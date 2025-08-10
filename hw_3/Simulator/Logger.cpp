#include "Logger.h"

#include <iostream>
#include <chrono>
#include <iomanip>
#include <sstream>

Logger &Logger::getInstance() {
    static Logger instance;
    return instance;
}

Logger::Logger() {
}

Logger::~Logger() {
    close();
}

void Logger::close() {
    if (log_file.is_open()) {
        log_file.close();
    }

    if (err_file.is_open()) {
        err_file.close();
    }

    if (input_err_file.is_open()) {
        input_err_file.close();
    }
}

void Logger::log(const std::string &message) {
    if (!log_file.is_open()) {
        log_file.open("log.txt", std::ios::out);
        if (!log_file.is_open()) {
            std::cerr << "Failed to open log file: log.txt" << std::endl;
            return;
        }
    }

    log_file << getTimestamp() << " - " << message << std::endl;
    log_file.flush();
}

void Logger::error(const std::string &message) {
    if (!err_file.is_open()) {
        err_file.open("errors.txt", std::ios::out);
        if (!err_file.is_open()) {
            std::cerr << "Failed to open error file: errors.txt" << std::endl;
            return;
        }
    }

    err_file << getTimestamp() << " - " << message << std::endl;
    err_file.flush();
}

void Logger::inputError(const std::string &message) {
    if (!input_err_file.is_open()) {
        input_err_file.open("input_errors.txt", std::ios::out);
        if (!input_err_file.is_open()) {
            std::cerr << "Failed to open input errors file: input_errors.txt" << std::endl;
            return;
        }
    }

    input_err_file << message << std::endl;
    input_err_file.flush();
}

std::string Logger::getTimestamp() {
    const auto now = std::chrono::system_clock::now();
    const auto time_t = std::chrono::system_clock::to_time_t(now);

    std::stringstream ss;
    ss << std::put_time(std::localtime(&time_t), "%Y-%m-%d %H:%M:%S");
    return ss.str();
}
