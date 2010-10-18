#ifndef LOGGER_H_
#define LOGGER_H_

#include <string>
#include <iostream>
#include <fstream>

class Logger {
public:
	Logger(char* outputFile);
	~Logger();

	void Log(const char* str);

private:
	std::ofstream out;
};

#endif //LOGGER_H_
