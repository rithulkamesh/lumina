#pragma once

#include <fstream>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <string>

enum LogLevel {
  LogLevel_Debug,
  LogLevel_Info,
  LogLevel_Warning,
  LogLevel_Error
};

class Logger {
public:
  static void InitLogger(const std::string &logFilePath);
  static void Log(LogLevel level, const std::string &message);

private:
  static std::ofstream logFile;
  static const char *GetLevelString(LogLevel level);
  static const char *GetColorCode(LogLevel level);
};
