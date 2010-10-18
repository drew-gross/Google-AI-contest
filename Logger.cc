#include "Logger.h"

Logger::Logger(char* outputFile) : out(outputFile) {
}

Logger::~Logger() {
	out.close();
}

void Logger::Log(const char* str) {
	if (enabled) {
		out << str << std::endl;
	}
}

void Logger::Enable() {
	enabled = true;
}

void Logger::Disable() {
	enabled = false;
}
