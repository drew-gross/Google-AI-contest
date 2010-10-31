#ifndef DONT_NEED_TO_DEFEND_EXCPETION
#define DONT_NEED_TO_DEFEND_EXCPETION

#include <stdexcept>

#include "Planet.h"

class DontNeedToDefendException : public std::runtime_error {
public:
	DontNeedToDefendException(Planet const * newPlanet);
	~DontNeedToDefendException() throw();

	Planet const * GetPlanet();
private:
	Planet const * planet;
};

#endif //DONT_NEED_TO_DEFEND_EXCPETION
