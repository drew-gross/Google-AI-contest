#include "Fleet.h"


Fleet::Fleet(int owner,
	int num_ships,
	int source_planet,
	int destination_planet,
	int total_trip_length,
	int turns_remaining) {
		owner_ = owner;
		num_ships_ = num_ships;
		source_planet_ = source_planet;
		destination_planet_ = destination_planet;
		total_trip_length_ = total_trip_length;
		turns_remaining_ = turns_remaining;
}

int Fleet::Owner() const {
	return owner_;
}

int Fleet::NumShips() const {
	return num_ships_;
}

int Fleet::SourcePlanet() const {
	return source_planet_;
}

int Fleet::DestinationPlanet() const {
	return destination_planet_;
}

int Fleet::TotalTripLength() const {
	return total_trip_length_;
}

int Fleet::TurnsRemaining() const {
	return turns_remaining_;
}