#include "ForceList.h"
#include <algorithm>
#include <vector>

int FleetList::ShipsFromPlayer( Player player ) const
{
	int numShips = 0;
	for (unsigned int i = 0; i < size(); i++) {
		numShips += (*this)[i]->ShipsFromPlayer(player);
	}
	return numShips;
}

void FleetList::DeleteAll() {
	for (unsigned int i = 0; i < size(); i++) {
		delete (*this)[i];
	}
	resize(0);
}

class isNotAttackingPlanet {
public:
	isNotAttackingPlanet(Planet const * p) : planet(p) {
	}
	bool operator()(Force * fleet) {
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

void FleetList::AddForce( Force * f )
{
	for (unsigned int i = 0; i < size(); ++i)
	{
		if (operator[](i)->ArrivesInTurns(f->TurnsRemaining()))
		{
			operator[](i)->AddForce(f);
			delete f;
			return;
		}
	}
	push_back(f);
}
