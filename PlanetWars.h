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

class PlanetWars {
public:
	// Initializes the PlanetWars singleton.
	static void Initialize(const std::string& game_state);

	// Uninitializes the PlanetWars singleton.
	static void Uninitialize();

	// The PlanetWars singleton.
	static PlanetWars& Instance();

protected:
	// Initializes the game state given a string containing game state data.
	PlanetWars(const std::string& game_state);

public:
	// Returns the number of planets on the map. Planets are numbered starting
	// with 0.
	int NumPlanets() const;

	// Returns the planet with the given planet_id. There are NumPlanets()
	// planets. They are numbered starting at 0.
	Planet& GetPlanet(int planet_id);

	// Returns the number of fleets.
	int NumFleets() const;

	// Returns the fleet with the given fleet_id. Fleets are numbered starting
	// with 0. There are NumFleets() fleets. fleet_id's are not consistent from
	// one turn to the next.
	const Fleet& GetFleet(int fleet_id) const;

	// Returns a list of all the planets.
	PlanetList Planets() const;

	// Return a list of all the planets owned by the current player. By
	// convention, the current player is always player number 1.
	PlanetList MyPlanets() const;

	// Return a list of all neutral planets.
	PlanetList NeutralPlanets() const;

	// Return a list of all the planets owned by rival players. This excludes
	// planets owned by the current player, as well as neutral planets.
	PlanetList EnemyPlanets() const;

	// Return a list of all the planets that are not owned by the current
	// player. This includes all enemy planets and neutral planets.
	PlanetList NotMyPlanets() const;

	// Return a list of all the fleets.
	FleetList Fleets() const;

	// Return a list of all the fleets owned by the current player.
	FleetList MyFleets() const;

	// Return a list of all the fleets owned by enemy players.
	FleetList EnemyFleets() const;

	// Add a fleet to the game state.
	void AddFleet(Fleet const & new_fleet);

	// Writes a string which represents the current game state. This string
	// conforms to the Point-in-Time format from the project Wiki.
	std::string ToString() const;

	// Returns the distance between two planets, rounded up to the next highest
	// integer. This is the number of discrete time steps it takes to get between
	// the two planets.
	static int Distance(Planet const & source_planet, Planet const & destination_planet);

	// Sends an order to the game engine. The order is to send num_ships ships
	// from source_planet to destination_planet. The order must be valid, or
	// else your bot will get kicked and lose the game. For example, you must own
	// source_planet, and you can't send more ships than you actually have on
	// that planet.
	void IssueOrder(Planet & source_planet, Planet const & destination_planet, int num_ships);

	// Returns true if the named player owns at least one planet or fleet.
	// Otherwise, the player is deemed to be dead and false is returned.
	bool IsAlive(int player_id) const;

	// Returns the number of ships that the given player has, either located
	// on planets or in flight.
	int NumShips(int player_id) const;

	// Sends a message to the game engine letting it know that you're done
	// issuing orders for now.
	void FinishTurn() const;

private:
	// The PlanetWars Singleton
	static PlanetWars* instance_;

	// Parses a game state from a string. On success, returns 1. On failure,
	// returns 0.
	int ParseGameState(const std::string& s);

	// Store all the planets and fleets. OMG we wouldn't wanna lose all the
	// planets and fleets, would we!?
	PlanetList planets_;
	FleetList fleets_;

	// Output log for sending orders to the engine.
	static Logger orders;
};

#endif
