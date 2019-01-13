#pragma once
#include <sstream>

class Logger {
private:
    std::stringstream stream;
    uint16_t bufferSize;
    bool enableDebug;
public:
    Logger(bool enableDebug);
    ~Logger();
    void setupLogFile();
    void logMessage(std::string message);
};
