#include "DontNeedToAttackException.h"

DontNeedToAttackException::DontNeedToAttackException(Planet const * newPlanet) : planet(newPlanet) {
}

DontNeedToAttackException::~DontNeedToAttackException() throw() {
}

Planet const * DontNeedToAttackException::GetPlanet() {
	return planet;
}
