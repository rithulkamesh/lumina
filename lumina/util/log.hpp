#pragma once

#include <fstream>
#include <iomanip>
#include <iostream>
#include <optional>
#include <sstream>
#include <string>

enum LogLevel {
  LogLevel_Debug,
  LogLevel_Info,
  LogLevel_Warning,
  LogLevel_Error,
  LogLevel_ValidationLayer
};

class Logger {
public:
  static void InitLogger(const std::string &logFilePath);
  static void Log(LogLevel level, const std::string &message, const char *file,
                  int line);

private:
  static std::optional<std::ofstream> logFile;
  static const char *GetLevelString(LogLevel level);
  static const char *GetColorTag(LogLevel level);
};
