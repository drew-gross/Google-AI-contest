#ifndef PLANET_LIST_H_
#define PLANET_LIST_H_

#include <vector>
#include "Planet.h"

class PlanetList : public std::vector<Planet*> {
public:
	Planet* Weakest();
	Planet* WeakestFromPlanet(Planet const & p);
	Planet* Strongest();
	void DeleteAll();
private:
};

#endif //PLANET_LIST_H_
