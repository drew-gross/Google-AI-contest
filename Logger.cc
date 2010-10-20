#include "Logger.h"

Logger::Logger(char* outputFile) : out(outputFile) {
	enabled = true;
}

Logger::~Logger() {
	out.close();
}

void Logger::Enable() {
	enabled = true;
}

void Logger::Disable() {
	enabled = false;
}
