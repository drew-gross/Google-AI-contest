#include "DontNeedToDefendException.h"

DontNeedToDefendException::DontNeedToDefendException(Planet const * newPlanet) : planet(newPlanet) {
}

DontNeedToDefendException::~DontNeedToDefendException() throw() {
}

Planet const * DontNeedToDefendException::GetPlanet() {
	return planet;
}
