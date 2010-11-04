#ifndef FLEET_LIST_H_
#define FLEET_LIST_H_

#include <vector>
#include <list>
#include "Fleet.h"
#include "Planet.h"

class FleetList : public std::vector<Fleet*> {
public:
	int NumShips();
	void DeleteAll();

	FleetList & OwnedBy(Player player);
	FleetList & AttackingPlanet( Planet const * p );
private:
};

#endif //FLEET_LIST_H_
