#include "PlanetList.h"

#include "PlanetWars.h"

#include <algorithm>

Planet const * PlanetList::Weakest() {
	if (size() == 0) {
		return nullptr;
	}
	int weakestIndex = 0;
	for (unsigned int i = 0; i < size(); ++i) {
		if ((*this)[weakestIndex].NumShips() > (*this)[i].NumShips()) {
			weakestIndex = i;
		}
	}
	return &(*this)[weakestIndex];
}

Planet const * PlanetList::Strongest() {
	if (size() == 0) {
		return nullptr;
	}
	int strongestIndex = 0;
	for (unsigned int i = 0; i < size(); ++i) {
		if ((*this)[strongestIndex].NumShips() < (*this)[i].NumShips()) {
			strongestIndex = i;
		}
	}
	return &(*this)[strongestIndex];
}
