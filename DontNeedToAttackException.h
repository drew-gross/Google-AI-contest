#ifndef DONT_NEED_TO_ATTACK_EXCPETION
#define DONT_NEED_TO_ATTACK_EXCPETION

#include <stdexcept>

#include "Planet.h"

class DontNeedToAttackException : public std::exception {
public:
	DontNeedToAttackException(Planet const * newPlanet);
	~DontNeedToAttackException();

	Planet const * GetPlanet();
private:
	Planet const * planet;
};

#endif //DONT_NEED_TO_ATTACK_EXCPETION
