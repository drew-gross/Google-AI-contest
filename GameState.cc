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

GameState* GameState::instance_ = nullptr;
Logger GameState::mapdata("orders.txt");

void GameState::Initialize() {
	if (instance_ != nullptr) {
		return;
	}
	instance_ = new GameState();
	instance_->turn = 0;
}

GameState& GameState::Instance() {
	Initialize();
	return *instance_;
}

GameState::GameState() {
}

int GameState::NumPlanets() const {
	return planets_.size();
}

Planet const * GameState::GetPlanet(int planet_id) {
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

void GameState::AddFleet(Fleet* new_fleet) {
	fleets_.push_back(new_fleet);
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

int GameState::Distance(Planet const * const source_planet, Planet const * const destination_planet) {
	double dx = source_planet->X() - destination_planet->X();
	double dy = source_planet->Y() - destination_planet->Y();
	return (int)ceil(sqrt(dx * dx + dy * dy));
}

int GameState::MaxDistance() {
	static int max_planet_separation_ = -1;
	if (max_planet_separation_ < 0) {
		for (unsigned int i = 0; i < Planets().size(); ++i) {
			for (unsigned int j = 0; j < Planets().size(); ++j) {
				max_planet_separation_ = std::max(max_planet_separation_, Distance(Planets()[i], Planets()[j]));
			}
		}
	}
	return max_planet_separation_;
}

void GameState::IssueOrder(Planet * const source_planet, Planet const * const destination_planet, int num_ships) {
	if (source_planet->PlanetID() == destination_planet->PlanetID()) throw std::runtime_error("Attempted to send ships from a planet to itself");
	if (num_ships > source_planet->NumShips()) {
		throw std::runtime_error("Not Enough Ships to send");
	}
	if (source_planet->Owner() != Player::self()) throw std::runtime_error("You don't own that planet");
	if (num_ships == 0) return;

	AddFleet(new Fleet(Player::self(), num_ships, source_planet->PlanetID(), destination_planet->PlanetID(), Distance(source_planet, destination_planet), Distance(source_planet, destination_planet)));
	destination_planet->ClearFutureCache();
	source_planet->RemoveShips(num_ships);

	std::cout << source_planet->PlanetID() << " " << destination_planet->PlanetID() << " " << num_ships << std::endl;
	std::cout.flush();
}

bool GameState::IsAlive(int player_id) const {
	for (unsigned int i = 0; i < planets_.size(); ++i) {
		if (planets_[i]->Owner() == player_id) {
			return true;
		}
	}
	for (unsigned int i = 0; i < fleets_.size(); ++i) {
		if (fleets_[i]->Owner() == player_id) {
			return true;
		}
	}
	return false;
}

int GameState::NumShips(int player_id) const {
	int num_ships = 0;
	for (unsigned int i = 0; i < planets_.size(); ++i) {
		if (planets_[i]->Owner() == player_id) {
			num_ships += planets_[i]->NumShips();
		}
	}
	for (unsigned int i = 0; i < fleets_.size(); ++i) {
		if (fleets_[i]->Owner() == player_id) {
			num_ships += fleets_[i]->NumShips();
		}
	}
	return num_ships;
}

int GameState::ParseGameState(const std::string& s) {
	mapdata.LogMapData(s);
	planets_.clear();
	fleets_.clear();
	std::vector<std::string> lines = StringUtil::Tokenize(s, "\n");
	int planet_id = 0;
	for (unsigned int i = 0; i < lines.size(); ++i) {
		std::string& line = lines[i];
		size_t comment_begin = line.find_first_of('#');
		if (comment_begin != std::string::npos) {
			line = line.substr(0, comment_begin);
		}
		std::vector<std::string> tokens = StringUtil::Tokenize(line);
		if (tokens.size() == 0) {
			continue;
		}
		if (tokens[0] == "P") {
			if (tokens.size() != 6) {
				throw std::runtime_error("error parsing gamestate");
			}
			Planet* p = new Planet(planet_id++,              // The ID of this planet
				Player(atoi(tokens[3].c_str())),  // Owner
				atoi(tokens[4].c_str()),  // Num ships
				atoi(tokens[5].c_str()),  // Growth rate
				atof(tokens[1].c_str()),  // X
				atof(tokens[2].c_str())); // Y
			planets_.push_back(p);
		} else if (tokens[0] == "F") {
			if (tokens.size() != 7) {
				throw std::runtime_error("error parsing gamestate");
			}
			Fleet* f = new Fleet(atoi(tokens[1].c_str()),  // Owner
				atoi(tokens[2].c_str()),  // Num ships
				atoi(tokens[3].c_str()),  // Source
				atoi(tokens[4].c_str()),  // Destination
				atoi(tokens[5].c_str()),  // Total trip length
				atoi(tokens[6].c_str())); // Turns remaining
			fleets_.push_back(f);
		} else {
			throw std::runtime_error("error parsing gamestate");
		}
	}
	return 1;
}

int GameState::MaxTurns()
{
	return 200;
}

int GameState::Turns()
{
	return turn;
}

int GameState::TurnsRemaining()
{
	return MaxTurns() - Turns();
}

void GameState::UnitTest()
{
	//none yet
}

void GameState::NextTurn()
{
	turn++;
}

void GameState::DeleteData()
{
	instance_->planets_.DeleteAll();
	instance_->fleets_.DeleteAll();
}
