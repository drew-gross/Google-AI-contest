#ifndef FLEET_LIST_H_
#define FLEET_LIST_H_

#include <vector>
#include <list>
#include "Force.h"
#include "Planet.h"

class FleetList : public std::vector<Force*> {
public:
	int ShipsFromPlayer(Player player) const;
	void DeleteAll();
	void AddForce(Force * f);

	FleetList & OwnedBy(Player player);
	FleetList & AttackingPlanet( Planet const * p );
private:
};

#endif //FLEET_LIST_H_
