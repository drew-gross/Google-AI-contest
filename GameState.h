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
	// Returns the number of planets on the map. Planets are numbered starting
	// with 0.
	int NumPlanets() const;

	// Returns the planet with the given planet_id. There are NumPlanets()
	// planets. They are numbered starting at 0.
	Planet const * GetPlanet(int planet_id) const;

	// Returns the number of fleets.
	int NumFleets() const;

	// Returns the fleet with the given fleet_id. Fleets are numbered starting
	// with 0. There are NumFleets() fleets. fleet_id's are not consistent from
	// one turn to the next.
	Fleet const * GetFleet(int fleet_id) const;

	PlanetList const & Planets() const;
	inline FleetList const & Fleets() const;

	void AddFleet(Fleet* new_fleet);
	void AddPlanet( Planet* p );

	// Returns the distance between two planets, rounded up to the next highest
	// integer. This is the number of discrete time steps it takes to get between
	// the two planets.
	static int Distance(Planet const * const source_planet, Planet const * const destination_planet);

	// Returns the maximum separation between planets.
	int MaxDistance() const;

	// Returns true if the named player owns at least one planet or fleet.
	// Otherwise, the player is deemed to be dead and false is returned.
	bool IsAlive(int player_id) const;

	// Returns the number of ships that the given player has, either located
	// on planets or in flight.
	int NumShips(int player_id) const;

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
