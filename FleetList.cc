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

class isNotOwnedBy {
public:
	isNotOwnedBy(Player newPlayer) : player(newPlayer) {
	}
	bool operator()(Fleet * fleet) {
		return fleet->Owner() != player;
	}
private:
	Player player;
};

FleetList & FleetList::OwnedBy( Player player )
{
	this->erase(remove_if(this->begin(), this->end(), isNotOwnedBy(player)), this->end());
	return *this;
}
