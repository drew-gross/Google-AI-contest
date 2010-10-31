#include "DontNeedToAttackException.h"

DontNeedToAttackException::DontNeedToAttackException(Planet const * newPlanet) :  runtime_error("DontNeedToAttackException"), planet(newPlanet) {
}

DontNeedToAttackException::~DontNeedToAttackException() throw() {
}

Planet const * DontNeedToAttackException::GetPlanet() {
	return planet;
}
