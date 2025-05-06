/***********************/
/** Custom C++ Logger **/
/***********************/

#include <ctime>
#include <fstream>
#include <iostream>
#include <sstream>

enum LogLevel { DEBUG, INFO, WARNING, ERROR, CRITICAL };

class StdoutLogger {
public:
    StdoutLogger(std::string name) {
        this->name = name;
    }

    void log(LogLevel level, const std::string& message)
    {
        time_t now = time(0);
        tm* timeinfo = localtime(&now);
        char timestamp[20];
        strftime(timestamp, sizeof(timestamp),
                 "%Y-%m-%d %H:%M:%S", timeinfo);

        std::ostringstream logEntry;
        logEntry << "[" << timestamp << "] "
                 << "[" << name << "]"
                 << levelToString(level) << ": "
                 << message
                 << std::endl;

        std::cout << logEntry.str();
    }

    void debug(const std::string& message) {
        this->log(LogLevel::DEBUG, message);
    }
    void info(const std::string& message) {
        this->log(LogLevel::INFO, message);
    }
    void warning(const std::string& message) {
        this->log(LogLevel::WARNING, message);
    }
    void error(const std::string& message) {
        this->log(LogLevel::ERROR, message);
    }
    void critical(const std::string& message) {
        this->log(LogLevel::CRITICAL, message);
    }

private:
    std::string name;
    std::string levelToString(LogLevel level)
    {
        switch (level) {
        case DEBUG:
            return "DEBUG";
        case INFO:
            return "INFO";
        case WARNING:
            return "WARNING";
        case ERROR:
            return "ERROR";
        case CRITICAL:
            return "CRITICAL";
        default:
            return "UNKNOWN";
        }
    }
};
