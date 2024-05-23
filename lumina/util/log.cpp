#include "log.hpp"

std::ofstream Logger::logFile;

void Logger::InitLogger(const std::string &logFilePath) {
  logFile.open(logFilePath, std::ios::out | std::ios::app);
}

void Logger::Log(LogLevel level, const std::string &message) {
  std::stringstream ss;
  ss << "[" << GetLevelString(level) << "] " << message << "\n";

  std::string logMessage = ss.str();

  std::cout << GetColorCode(level) << logMessage << "\033[0m";

  if (logFile.is_open()) {
    logFile << logMessage;
    logFile.flush();
  }
}

const char *Logger::GetLevelString(LogLevel level) {
  switch (level) {
  case LogLevel_Debug:
    return "DEBUG";
  case LogLevel_Info:
    return "INFO";
  case LogLevel_Warning:
    return "WARNING";
  case LogLevel_Error:
    return "ERROR";
  default:
    return "UNKNOWN";
  }
}

const char *Logger::GetColorCode(LogLevel level) {
  switch (level) {
  case LogLevel_Debug:
    return "\033[36m";
  case LogLevel_Info:
    return "\033[32m";
  case LogLevel_Warning:
    return "\033[33m";
  case LogLevel_Error:
    return "\033[31m";
  default:
    return "\033[0m";
  }
}
