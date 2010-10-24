#ifndef DONT_NEED_TO_DEFEND_EXCPETION
#define DONT_NEED_TO_DEFEND_EXCPETION

#include <stdexcept>

#include "Planet.h"

class DontNeedToDefendException : public std::exception {
public:
	DontNeedToDefendException(Planet const * newPlanet);
	~DontNeedToDefendException();

	Planet const * GetPlanet();
private:
	Planet const * planet;
};

#endif //DONT_NEED_TO_DEFEND_EXCPETION
