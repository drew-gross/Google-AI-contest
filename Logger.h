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
	
	void Enable();
	void Disable();

private:
	std::ofstream out;
	bool enabled;
};

#endif //LOGGER_H_
