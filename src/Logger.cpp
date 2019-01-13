#include "Logger.hpp"
#include <fstream>
using namespace std;

Logger::Logger(bool enableDebug) : enableDebug(enableDebug) {

}

Logger::~Logger() {

}

void Logger::setupLogFile() {
    if (!enableDebug) {
        return;
    }
    remove("nanacpp.log");
}

void Logger::logMessage(std::string message) {
    if (!enableDebug) {
        return;
    }
    this->stream << message << std::endl;
    this->bufferSize++;
    if (bufferSize < 8192) {
        return;
    }
    std::ofstream logfile;
    logfile.open("nanacpp.log", ios::out | ios::app);
    logfile << this->stream.str();
    logfile.close();
    this->stream.str(std::string());
    this->bufferSize = 0;
}
