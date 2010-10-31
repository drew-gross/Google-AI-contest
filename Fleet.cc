#include "Fleet.h"
#include "GameState.h"
#include "GameManager.h"


Fleet::Fleet(Player owner,
	int num_ships,
	int source_planet,
	int destination_planet,
	int total_trip_length,
	unsigned int turns_remaining) : owner_(owner),
									num_ships_(num_ships), 
									source_planet_(source_planet), 
									destination_planet_(destination_planet), 
									total_trip_length_(total_trip_length), 
									turns_remaining_(turns_remaining) {
}

Player Fleet::Owner() const {
	return owner_;
}

int Fleet::NumShips() const {
	return num_ships_;
}

Planet const * Fleet::SourcePlanet() const {
	return GameManager::Instance().State().GetPlanet(source_planet_);
}

Planet const * Fleet::DestinationPlanet() const {
	return GameManager::Instance().State().GetPlanet(destination_planet_);
}

int Fleet::TotalTripLength() const {
	return total_trip_length_;
}

unsigned int Fleet::TurnsRemaining() const {
	return turns_remaining_;
}
