#ifndef LOGGER_H_
#define LOGGER_H_

#include <string>
#include <iostream>
#include <fstream>

class Logger {
public:
	Logger(char* outputFile);
	~Logger();

	template<typename T>
	void Log(T str);
	
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

#endif //LOGGER_H_
