#include "Logger.h"

Logger::Logger(std::string outputFile) : out(outputFile) {
}

Logger::~Logger() {
	out.close();
}

void Logger::Log(std::string str) {
	out << str << std::endl;
}