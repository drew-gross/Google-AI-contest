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
	return planets_.at(planet_id);
}

PlanetList const & GameState::Planets() const {
	return planets_;
}

void GameState::AddForce(Force* f) {
	fleets_.push_back(f);
	f->DestinationPlanet()->ClearFutureCache();
}

int GameState::MaxDistance() const {
	static int max_planet_separation_ = -1;
	if (max_planet_separation_ < 0) {
		for (unsigned int i = 0; i < Planets().size(); ++i) {
			for (unsigned int j = 0; j < Planets().size(); ++j) {
				max_planet_separation_ = std::max(max_planet_separation_, Planets()[i]->DistanceTo(Planets()[j]));
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
