#include "FleetList.h"
#include <algorithm>

int FleetList::NumShips() {
	int numShips = 0;
	for (unsigned int i = 0; i < size(); i++) {
		numShips += (*this)[i]->Ships();
	}
	return numShips;
}

void FleetList::DeleteAll() {
	for (unsigned int i = 0; i < size(); i++) {
		delete (*this)[i];
	}
	resize(0);
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
	erase(remove_if(begin(), end(), isNotOwnedBy(player)), end());
	return *this;
}
class isNotAttackingPlanet {
public:
	isNotAttackingPlanet(Planet const * p) : planet(p) {
	}
	bool operator()(Fleet * fleet) {
		return *(fleet->DestinationPlanet()) != *planet;
	}
private:
	Planet const * planet;
};

FleetList & FleetList::AttackingPlanet( Planet const * p )
{
	erase(remove_if(begin(), end(), isNotAttackingPlanet(p)), end());
	return *this;
}
