#include "GameState.h"
#include "Utilities.h"
#include "StringUtil.h"

#include "DontNeedToAttackException.h"
#include "DontNeedToDefendException.h"

#include <cmath>
#include <cstdlib>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <exception>
#include <stdexcept>
#include "GameManager.h"


int GameState::NumPlanets() const {
	return planets_.size();
}

Planet const * GameState::GetPlanet(int planet_id) const {
	return planets_[planet_id];
}

int GameState::NumFleets() const {
	return fleets_.size();
}

Fleet const * GameState::GetFleet(int fleet_id) const {
	return fleets_[fleet_id];
}

PlanetList const & GameState::Planets() const {
	return planets_;
}

void GameState::AddFleet(Fleet* f) {
	fleets_.push_back(f);
}

std::string GameState::ToString() const {
	std::stringstream s;
	for (unsigned int i = 0; i < planets_.size(); ++i) {
		Planet* p = planets_[i];
		s << "P " << p->X() << " " << p->Y() << " " << p->Owner()
			<< " " << p->NumShips() << " " << p->GrowthRate() << std::endl;
	}
	for (unsigned int i = 0; i < fleets_.size(); ++i) {
		Fleet* f = fleets_[i];
		s << "F " << f->Owner() << " " << f->NumShips() << " "
			<< f->SourcePlanet() << " " << f->DestinationPlanet() << " "
			<< f->TotalTripLength() << " " << f->TurnsRemaining() << std::endl;
	}
	return s.str();
}

int GameState::MaxDistance() const {
	static int max_planet_separation_ = -1;
	if (max_planet_separation_ < 0) {
		for (unsigned int i = 0; i < Planets().size(); ++i) {
			for (unsigned int j = 0; j < Planets().size(); ++j) {
				max_planet_separation_ = std::max(max_planet_separation_, GameState::Distance(Planets()[i], Planets()[j]));
			}
		}
	}
	return max_planet_separation_;
}

void GameState::DeleteData()
{
	planets_.DeleteAll();
	fleets_.DeleteAll();
}

void GameState::AddPlanet( Planet* p )
{
	planets_.push_back(p);
}

int GameState::Distance(Planet const * const source_planet, Planet const * const destination_planet) {
	double dx = source_planet->X() - destination_planet->X();
	double dy = source_planet->Y() - destination_planet->Y();
	return (int)ceil(sqrt(dx * dx + dy * dy));
}
