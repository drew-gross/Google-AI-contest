#ifndef PLANET_LIST_H_
#define PLANET_LIST_H_

#include <vector>
#include <list>
#include "Player.h"

class Planet;

class PlanetList : public std::vector<Planet*> {
public:
	void RemoveMatches(Planet* p);

	Planet* Weakest();
	Planet* WeakestFromPlanet(Planet const * const p);
	Planet* Strongest();

	int ShipsAvailable();

	PlanetList OwnedBy(Player player) const;
	PlanetList NotOwnedBy(Player player) const;
	PlanetList NeedDefending() const;
	PlanetList NeedAttacking() const;
	PlanetList Fronts() const;

	void DeleteAll();
	Planet* HighestROIFromPlanet(Planet const * source) const;
private:
};

#endif //PLANET_LIST_H_
