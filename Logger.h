#ifndef LOGGER_H_
#define LOGGER_H_

#include <string>
#include <iostream>
#include <fstream>

class Logger {
public:
	Logger(std::string outputFile);
	~Logger();

	void Log(std::string str);

private:
	std::ofstream out;
};

#endif //LOGGER_H_
