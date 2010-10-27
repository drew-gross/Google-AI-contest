#ifndef PLANET_LIST_H_
#define PLANET_LIST_H_

#include <vector>
#include "Planet.h"

class PlanetList : public std::vector<Planet*> {
public:
	Planet* Weakest();
	Planet* WeakestFromPlanet(Planet const & p);
	Planet* Strongest();

	int NumShipsAvailable();

	PlanetList OwnedBy(Player player) const;
	PlanetList NotOwnedBy(Player player) const;
	PlanetList NeedDefending() const;

	void DeleteAll();
	Planet* HighestROIFromPlanet(Planet const * source) const;
private:
};

#endif //PLANET_LIST_H_
