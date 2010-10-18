#include "Logger.h"

Logger::Logger(char* outputFile) : out(outputFile) {
}

Logger::~Logger() {
	out.close();
}

void Logger::Log(const char* str) {
	out << str << std::endl;
}