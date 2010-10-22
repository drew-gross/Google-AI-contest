#ifndef UTILITIES_H_
#define UTILITIES_H_

#include <algorithm>

#ifndef nullptr
#define nullptr 0
#endif

enum Player {
	 neutral = 0,
	 self = 1,
	 enemy = 2
};

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

#endif //UTISITIES_H_
