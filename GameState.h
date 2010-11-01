#ifndef PLANET_WARS_H_
#define PLANET_WARS_H_

// This file contains helper code that does all the boring stuff for you.
// The code in this file takes care of storing lists of planets and fleets, as
// well as communicating with the game engine. You can get along just fine
// without ever looking at this file. However, you are welcome to modify it
// if you want to.

#include "Planet.h"
#include "PlanetList.h"
#include "Fleet.h"
#include "FleetList.h"

#include "Logger.h"

#include <string>
#include <vector>

class GameState {
public:
	int NumPlanets() const;
	int NumFleets() const;

	// Returns the planet with the given planet_id. Planets are numbered starting at 0.
	Planet const * GetPlanet(int planet_id) const;

	// Returns the fleet with the given fleet_id. Fleets are numbered starting at 0. fleet_id's are not consistent from one turn to the next.
	Fleet const * GetFleet(int fleet_id) const;

	PlanetList const & Planets() const;
	inline FleetList const & Fleets() const;

	void AddFleet(Fleet* f);
	void AddPlanet(Planet* p);

	// Returns the maximum separation between planets.
	int MaxDistance() const;

	// Writes a string which represents the current game state. This string
	// conforms to the Point-in-Time format from the project Wiki.
	std::string ToString() const;

private:
	friend class GameManager;
	friend class AI;
	// Deletes the Planets and Fleets
	void DeleteData();

	PlanetList planets_;
	FleetList fleets_;
};

FleetList const & GameState::Fleets() const {
	return fleets_;
}

#endif
