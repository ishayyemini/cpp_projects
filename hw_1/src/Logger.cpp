#include "Logger.h"
#include <iostream>
#include <chrono>
#include <iomanip>
#include <sstream>

Logger &Logger::getInstance() {
    static Logger instance;
    return instance;
}

Logger::Logger() : initialized(false) {
}

Logger::~Logger() {
    close();
}

bool Logger::init(const std::string &log_file_path, const std::string &err_file_path,
                  const std::string &input_err_file_path) {
    if (initialized) {
        close();
    }

    log_file.open(log_file_path, std::ios::out);
    if (!log_file.is_open()) {
        std::cerr << "Failed to open log file: " << log_file_path << std::endl;
        return false;
    }

    err_file.open(err_file_path, std::ios::out);
    if (!err_file.is_open()) {
        std::cerr << "Failed to open error file: " << err_file_path << std::endl;
        log_file.close();
        return false;
    }

    this->input_err_file_path = input_err_file_path;

    initialized = true;
    return true;
}

void Logger::close() {
    if (log_file.is_open()) {
        log_file.close();
    }

    if (err_file.is_open()) {
        err_file.close();
    }

    initialized = false;
}

void Logger::log(const std::string &message) {
    if (!initialized) {
        std::cerr << "Logger not initialized, message: " << message << std::endl;
        return;
    }

    log_file << getTimestamp() << " - " << message << std::endl;
    log_file.flush();
}

void Logger::error(const std::string &message) {
    if (!initialized) {
        std::cerr << "Logger not initialized, error: " << message << std::endl;
        return;
    }

    err_file << getTimestamp() << " - " << message << std::endl;
    err_file.flush();
}

void Logger::inputError(const std::string &message) {
    if (!input_err_file.is_open()) {
        input_err_file.open(input_err_file_path, std::ios::out);
    }

    if (!input_err_file.is_open()) {
        std::cerr << "Failed to open input errors file: " << input_err_file_path << std::endl;
    }

    input_err_file << message << std::endl;
    input_err_file.flush();
}

std::string Logger::getTimestamp() const {
    auto now = std::chrono::system_clock::now();
    auto time_t = std::chrono::system_clock::to_time_t(now);

    std::stringstream ss;
    ss << std::put_time(std::localtime(&time_t), "%Y-%m-%d %H:%M:%S");
    return ss.str();
}
