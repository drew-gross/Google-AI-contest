#ifndef FLEET_H_
#define FLEET_H_

// This class stores details about one fleet. There is one of these classes
// for each fleet that is in flight at any given time.
class Fleet {
public:
	// Initializes a fleet.
	Fleet(int owner,
		int num_ships,
		int source_planet,
		int destination_planet,
		int total_trip_length,
		unsigned int turns_remaining);

	// Returns the playerID of the owner of the fleet. Your player ID is always
	// 1. So if the owner is 1, you own the fleet. If the owner is 2 or some
	// other number, then this fleet belongs to your enemy.
	int Owner() const;

	// Returns the number of ships that comprise this fleet.
	int NumShips() const;

	// Returns the ID of the planet where this fleet originated.
	int SourcePlanet() const;

	// Returns the ID of the planet where this fleet is headed.
	int DestinationPlanet() const;

	// Returns the total distance that is being traveled by this fleet. This
	// is the distance between the source planet and the destination planet,
	// rounded up to the nearest whole number.
	int TotalTripLength() const;

	// Returns the number of turns until this fleet reaches its destination. If
	// this value is 1, then the fleet will hit the destination planet next turn.
	unsigned int TurnsRemaining() const;

private:
	int owner_;
	int num_ships_;
	int source_planet_;
	int destination_planet_;
	int total_trip_length_;
	int turns_remaining_;
};

#endif //FLEET_H
