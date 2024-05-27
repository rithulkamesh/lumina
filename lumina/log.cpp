#include "log.hpp"
#include <chrono>
#include <iomanip>

std::optional<std::ofstream> Logger::logFile;

void Logger::InitLogger(const std::string &logFilePath) {
  if (!logFilePath.empty()) {
    logFile.emplace(logFilePath, std::ios::out | std::ios::app);
  }
}

void Logger::Log(LogLevel level, const std::string &message, const char *file,
                 int line) {
  auto now = std::chrono::system_clock::now();
  auto time = std::chrono::system_clock::to_time_t(now);
  std::stringstream ss;
  ss << std::put_time(std::localtime(&time), "%Y.%m.%d-%H:%M:%S.")
     << std::setfill('0') << std::setw(3)
     << std::chrono::duration_cast<std::chrono::milliseconds>(
            now.time_since_epoch())
                .count() %
            1000
     << " - " << GetColorTag(level) << GetLevelString(level) << "\033[0m"
     << " [" << file << ":" << line << "] " << message << "\n";

  std::cout << ss.str();

  if (logFile.has_value() && logFile->is_open()) {
    *logFile << ss.str();
    logFile->flush();
  }
}

const char *Logger::GetLevelString(LogLevel level) {
  switch (level) {
  case LogLevel_Debug:
    return "<trace>";
  case LogLevel_Info:
    return "<info>";
  case LogLevel_Warning:
    return "<warning>";
  case LogLevel_Error:
    return "<error>";
  default:
    return "<unknown>";
  }
}

const char *Logger::GetColorTag(LogLevel level) {
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
