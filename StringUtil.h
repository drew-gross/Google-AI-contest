#ifndef STRING_UTIL_H_
#define STRING_UTIL_H_

#include <string>
#include <vector>

// This is a utility class that parses strings.
class StringUtil {
public:
	// Tokenizes a string s into tokens. Tokens are delimited by any of the
	// characters in delimiters. Blank tokens are omitted.
	static void Tokenize(const std::string& s,
		const std::string& delimiters,
		std::vector<std::string>& tokens);

	// A more convenient way of calling the Tokenize() method.
	static std::vector<std::string> Tokenize(
		const std::string& s,
		const std::string& delimiters = std::string(" "));
};

#endif //STRING_UTIL_H_