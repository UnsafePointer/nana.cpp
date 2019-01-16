#pragma once
#include <sstream>

class Logger {
private:
    std::stringstream stream;
    uint16_t bufferSize;
public:
    Logger(bool enableDebug);
    ~Logger();
    bool enableDebug;
    void setupLogFile();
    void logMessage(std::string message);
};
