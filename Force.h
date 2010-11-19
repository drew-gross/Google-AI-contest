#ifndef FORCE_H_
#define FORCE_H_

#include "Planet.h"
#include <stdexcept>

// This class stores details about one fleet. There is one of these classes
// for each fleet that is in flight at any given time.
class Force {
public:
	// Initializes a fleet.
	Force(Player owner,
		int num_ships,
		Planet const * destination_planet,
		unsigned int turns_remaining);

	// Returns the number of ships that comprise this fleet.
	int PlayerShips() const;
	int EnemyShips() const;
	int ShipsFromPlayer(Player player) const;

	// Returns the ID of the planet where this fleet is headed.
	Planet const* DestinationPlanet() const;

	// Returns the number of turns until this fleet reaches its destination. If
	// this value is 1, then the fleet will hit the destination planet next turn.
	unsigned int TurnsRemaining() const;

	// Returns true if the fleet will be arriving at its target in the specified amount of turns.
	inline bool ArrivesInTurns(unsigned int turns) const;

	void AddForce(Force const * const f);

private:
	int playerShips;
	int enemyShips;
	Planet const * destination_planet_;
	unsigned int turns_remaining_;
};

bool Force::ArrivesInTurns(unsigned int turns) const {
	return (TurnsRemaining() - 1 == turns);
}

#endif //FORCE_H
