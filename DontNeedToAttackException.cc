#include "DontNeedToAttackException.h"

DontNeedToAttackException::DontNeedToAttackException(Planet const * newPlanet) : planet(newPlanet) {
}

DontNeedToAttackException::~DontNeedToAttackException() {
}

Planet const * DontNeedToAttackException::getPlanet() {
	return planet;
}