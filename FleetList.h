#ifndef FLEET_LIST_H_
#define FLEET_LIST_H_

#include <vector>
#include "Fleet.h"
#include "Planet.h"

class FleetList : public std::vector<Fleet*> {
public:
	int NumShips();
	void DeleteAll();
private:
};

#endif //FLEET_LIST_H_
