#ifndef LOGGER_H_
#define LOGGER_H_

#include <string>
#include <iostream>
#include <fstream>

#include "Utilities.h"

#define LogVar(var) MacroHelper1(toString(var), var)
#define CreateLogger(filename) Logger filename(toString(filename)".txt")
#define CreateStaticLogger(filename) static Logger filename(toString(filename)".txt")

class Logger {
public:
	Logger(char const * outputFile);
	~Logger();
	
	template<typename T>
	void Log(T str);

	template<typename T, typename U>
	void Log(T str1, U str2);

	void LogMapData(std::string mapData);

	template<typename T, typename U>
	void MacroHelper1(T str1, U str2);
	
	void Enable();
	void Disable();

private:
	std::ofstream out;
	bool enabled;
};

template<typename T>
void Logger::Log(T str) {
	if (enabled) {
		out << str << std::endl;
	}
}

template<typename T, typename U>
void Logger::Log(T str1, U str2) {
	if (enabled) {
		out << str1 << str2 << std::endl;
	}
}

template<typename T, typename U>
void Logger::MacroHelper1(T varName, U var) {
	if (enabled) {
		out << "Value of " << varName << ": " << var << std::endl;
	}
}

#endif //LOGGER_H_
