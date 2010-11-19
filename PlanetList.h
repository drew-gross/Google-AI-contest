#ifndef PLANET_LIST_H_
#define PLANET_LIST_H_

#include <vector>
#include <list>
#include "Player.h"

class Planet;

class PlanetList : public std::vector<Planet*> {
public:

	void Remove(Planet const * const p);

	Planet* Weakest();
	Planet* Strongest();

	int Ships() const;
	int ShipsAvailable();

	typedef bool (Planet::*Set)(void) const;
	PlanetList Subset(PlanetList::Set matcher) const;

	typedef bool (Planet::*PlayerSet)(Player) const;
	PlanetList PlayerSubset(PlanetList::PlayerSet matcher, Player player) const;

	PlanetList OwnedInTurnsBy( Player player, int turns ) const;

	void SortByHighestGrowth();

	void DeleteAll();

	typedef Planet* (PlanetList::*Prioritiser)(Planet const * const) const;
	Planet* HighestROIFromPlanet(Planet const * const source) const;
	Planet* HighestGrowthEnemy(Planet const * const source) const;
	Planet* WeakestFromPlanet(Planet const * const source) const;

private:
};

#endif //PLANET_LIST_H_
