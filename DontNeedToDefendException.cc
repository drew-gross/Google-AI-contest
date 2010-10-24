#include "DontNeedToDefendException.h"

DontNeedToDefendException::DontNeedToDefendException(Planet const * newPlanet) : planet(newPlanet) {
}

DontNeedToDefendException::~DontNeedToDefendException() {
}

Planet const * DontNeedToDefendException::GetPlanet() {
	return planet;
}