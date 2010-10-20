#ifndef PLANET_H_
#define PLANET_H_

// Stores information about one planet. There is one instance of this class
// for each planet on the map.
class Planet {
public:
	// Initializes a planet.
	Planet(int planet_id,
		int owner,
		int num_ships,
		int growth_rate,
		double x,
		double y);

	// Returns the ID of this planets. Planets are numbered starting at zero.
	int PlanetID() const;

	// Returns the ID of the player that owns this planet. Your playerID is
	// always 1. If the owner is 1, this is your planet. If the owner is 0, then
	// the planet is neutral. If the owner is 2 or some other number, then this
	// planet belongs to the enemy.
	int Owner() const;

	// The number of ships on the planet. This is the "population" of the planet.
	int NumShips() const;

	// The number of ships on the planet in the specified amount of turns.
	int NumShipsInTurns(unsigned int turns) const;

	// Returns the growth rate of the planet. Unless the planet is neutral, the
	// population of the planet grows by this amount each turn. The higher this
	// number is, the faster this planet produces ships.
	int GrowthRate() const;

	// The position of the planet in space.
	double X() const;
	double Y() const;

	// Use the following functions to set the properties of this planet. Note
	// that these functions only affect your program's copy of the game state.
	// You can't steal your opponent's planets just by changing the owner to 1
	// using the Owner(int) function! :-)
	void Owner(int new_owner);
	void NumShips(int new_num_ships);
	void AddShips(int amount);
	void RemoveShips(int amount);

	// Compares planets by their ID's.
	bool operator==(Planet const & rhs);

private:
	int planet_id_;
	int owner_;
	int num_ships_;
	int growth_rate_;
	double x_, y_;
};

#endif //PLANET_H_
