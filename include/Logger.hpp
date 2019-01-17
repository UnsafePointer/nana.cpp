#pragma once
#include <sstream>

class Logger {
#ifndef __SWITCH__
private:
    std::stringstream stream;
    uint16_t bufferSize;
#endif
public:
    Logger(bool enableDebug);
    ~Logger();
    bool enableDebug;
    void setupLogFile();
    void logMessage(std::string message);
};
