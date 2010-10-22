#include "FleetList.h"
#include <algorithm>

int FleetList::NumShips() {
	int numShips = 0;
	for (unsigned int i = 0; i < size(); i++) {
		numShips += (*this)[i]->NumShips();
	}
	return numShips;
}

void FleetList::DeleteAll() {
	for (unsigned int i = 0; i < size(); i++) {
		delete (*this)[i];
	}
	this->resize(0);
}
