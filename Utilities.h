#ifndef UTILITIES_H_
#define UTILITIES_H_

#define NEUTRAL 0
#define SELF 1
#define ENEMY 2

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
