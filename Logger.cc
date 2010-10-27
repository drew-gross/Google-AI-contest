#include "Logger.h"

Logger::Logger(char const * outputFile) : out(outputFile) {
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

void Logger::LogMapData( std::string mapData )
{
	if (enabled) {
		out << mapData << "go" << std::endl << std::endl << std::endl;
	}
}
