#ifndef PLANET_H_
#define PLANET_H_

// Stores information about one planet. There is one instance of this class
// for each planet on the map.
#include <utility>
#include <vector>

#include "Utilities.h"
#include "PlanetState.h"

class PlanetList;

class Planet {
public:
	// Initializes a planet.
	Planet(int planet_id,
		PlanetState newState,
		int growth_rate,
		double x,
		double y);

	// Returns the ID of this planets. Planets are numbered starting at zero.
	int PlanetID() const;
	Player Owner() const;
	Player OwnerInTurns(unsigned int turnsInFuture) const;
	int Ships() const;

	int DistanceTo( Planet const * p ) const;

	// The number of ships that can be sent without causing loss of control of the planet.
	int ShipsAvailable();

	// Returns true if the closest enemy planet is twice as far as the closest friendly planet
	bool IsSupplier();

	// The number of ships on the planet in the specified amount of turns.
	int ShipsInTurns(unsigned int turnsInFuture) const;

	// The number of ships you need to arrive at the specified number of turns to take the planet over.
	int ShipsToTakeoverInTurns(unsigned int turnsInFuture) const;

	// Calculates the time when the planet will have the lowest amount of enemy ships on it.
	// Throws an exception if I will own the planet at some point
	int OptimalAttackTime() const;

	// Calculates the turn immediately before when the planet will get taken over by the enemy.
	int OptimalDefenseTime() const;

	// Determines whether the planet needs to be defended. Returns true if I own 
	// the planet or will own it at some point in time and the
	// enemy will eventually own the planet given the current game state
	bool NeedToDefend() const;

	// Determines whether the planet needs to be attacked. Returns true if
	// I will not own the planet indefinitely
	bool NeedToAttack() const;

	// Takes the list of planets specified and requests defense from them until the planet no longer needs defense
	void SeekDefenseFrom(PlanetList &defendersAtOptimalTime, int optimalDefenseTime);

	// Sends all available ships to the target planet
	void Reinforce(Planet const * p);

	// Sends enough ships to takeover the planet, or the maximum available
	void AttemptToTakeover(Planet const * p);

	// Returns true if this planet has enough ships to takeover p
	bool CanTakeover( Planet const* p );

	// Returns the growth rate of the planet. Unless the planet is neutral, the
	// population of the planet grows by this amount each turn. The higher this
	// number is, the faster this planet produces ships.
	int GrowthRate() const;

	// Returns the return over the course of the game of taking a planet over
	// in the specified amount of turns
	int ReturnOnInvestment(int turns);
	int NeutralROI(int turns);
	int EnemyROI(int turns);

	Planet * ClosestPlanet();
	Planet * ClosestPlanetInList(PlanetList list);
	Planet const * ClosestPlanetOwnedBy( Player player ) const;

	// Adds up all the ships from fleets owned by the specified player and arriving
	// In the specified number of turns
	int ShipsArrivingInTurns(Player fromPlayer, int numTurns) const;

	// The position of the planet in space.
	double X() const;
	double Y() const;

	void AddShips(int amount);
	void RemoveShips(int amount);

	// Compares planets by their ID's.
	bool operator==(Planet const & rhs) const;
	bool operator!=(Planet const & rhs) const;

	void ClearFutureCache() const;

private:
	PlanetState StateInTurns(unsigned int turns) const;
	std::vector<PlanetState> const & FutureStates(unsigned int turns) const;
	PlanetState CurrentState() const;
	PlanetState state;
	int planet_id_;
	int growth_rate_;
	double x_, y_;

	mutable std::vector<PlanetState> stateInFuture;
};

#endif //PLANET_H_
