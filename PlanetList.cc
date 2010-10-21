#include "PlanetList.h"

#include "PlanetWars.h"
#include "Utilities.h"

#include <algorithm>

Planet* PlanetList::Weakest() {
	if (size() == 0) return nullptr;
	Planet* weakestPlanet = (*this)[0];
	for (unsigned int i = 0; i < size(); ++i) {
		Planet* curPlanet = (*this)[i];
		if (curPlanet->NumShips() < weakestPlanet->NumShips()) {
			weakestPlanet = curPlanet;
		}
	}
	return weakestPlanet;
}

Planet* PlanetList::Strongest() {
	if (size() == 0) return nullptr;
	Planet* strongestPlanet = (*this)[0];
	for (unsigned int i = 0; i < size(); ++i) {
		Planet* curPlanet = (*this)[i];
		if (curPlanet->NumShips() > strongestPlanet->NumShips()) {
			strongestPlanet = curPlanet;
		}
	}
	return strongestPlanet;
}

void PlanetList::DeleteAll() {
	for (unsigned int i = 0; i < size(); i++) {
		delete (*this)[i];
	}
	this->resize(0);
}
