#include "DontNeedToDefendException.h"

DontNeedToDefendException::DontNeedToDefendException(Planet const * newPlanet) : runtime_error("DontNeedToDefendException"), planet(newPlanet) {
}

DontNeedToDefendException::~DontNeedToDefendException() throw() {
}

Planet const * DontNeedToDefendException::GetPlanet() {
	return planet;
}
