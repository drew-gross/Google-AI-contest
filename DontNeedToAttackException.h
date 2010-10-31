#ifndef DONT_NEED_TO_ATTACK_EXCPETION
#define DONT_NEED_TO_ATTACK_EXCPETION

#include <stdexcept>

#include "Planet.h"

class DontNeedToAttackException : public std::runtime_error {
public:
	DontNeedToAttackException(Planet const * newPlanet);
	~DontNeedToAttackException() throw();

	Planet const * GetPlanet();
private:
	Planet const * planet;
};

#endif //DONT_NEED_TO_ATTACK_EXCPETION
