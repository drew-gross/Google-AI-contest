#ifndef UTILITIES_H_
#define UTILITIES_H_

#include <algorithm>
#include <vector>
#include <fstream>
#include <iostream>

#ifndef nullptr
#define nullptr 0
#endif

#define toString(x) #x

namespace std {
	template<typename T>
	T max(T first, T second, T third) {
		return max(max(first, second), third);
	}

	template<typename T>
	T median(T first, T second, T third) {
		if ((first <= second && second <= third) || (first >= second && second >= third)) return second;
		if ((second <= first && first <= third) || (second >= first && first >= third)) return first;
		if ((first <= third && third <= second) || (first >= third && third >= second)) return third;
	}
}


template<typename T, typename U> 
std::ostream& operator<<(std::ostream& out, const std::pair<T, U>& pair) {
  out << "first: " << pair.first << ", second: " << pair.second;
  return out;
}

template<typename T>
std::ostream& operator<<(std::ostream& out, std::vector<T> vec) {
	out << std::endl << "{" << std::endl;
	for (unsigned int i = 0; i < vec.size(); ++i) {
		out << vec[i] << "," << std::endl;
	}
	out << "}" << std::endl;
	return out;
}

#endif //UTILITIES_H_
