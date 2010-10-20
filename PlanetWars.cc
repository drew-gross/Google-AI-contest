#include "Utilities.h"
#include "PlanetWars.h"
#include "StringUtil.h"

#include <cmath>
#include <cstdlib>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <exception>

PlanetWars* PlanetWars::instance_ = nullptr;
Logger PlanetWars::orders = Logger("orders.txt");

void PlanetWars::Initialize(const std::string& game_state) {
	if (instance_ != nullptr) return;
	instance_ = new PlanetWars(game_state);
}

void PlanetWars::Uninitialize() {
	delete instance_;
	instance_ = nullptr;
}

PlanetWars& PlanetWars::Instance() {
	if (instance_ != nullptr) {
		return *instance_;
	} else {
		throw std::runtime_error("Trying to use an uninitialised PlanetWars singleton!");
	}
}

PlanetWars::PlanetWars(const std::string& gameState) {
	ParseGameState(gameState);
}

int PlanetWars::NumPlanets() const {
	return planets_.size();
}

Planet& PlanetWars::GetPlanet(int planet_id) {
	return planets_[planet_id];
}

int PlanetWars::NumFleets() const {
	return fleets_.size();
}

const Fleet& PlanetWars::GetFleet(int fleet_id) const {
	return fleets_[fleet_id];
}

std::vector<Planet> PlanetWars::Planets() const {
	std::vector<Planet> r;
	for (unsigned int i = 0; i < planets_.size(); ++i) {
		const Planet& p = planets_[i];
		r.push_back(p);
	}
	return r;
}

std::vector<Planet> PlanetWars::MyPlanets() const {
	std::vector<Planet> r;
	for (unsigned int i = 0; i < planets_.size(); ++i) {
		const Planet& p = planets_[i];
		if (p.Owner() == 1) {
			r.push_back(p);
		}
	}
	return r;
}

std::vector<Planet> PlanetWars::NeutralPlanets() const {
	std::vector<Planet> r;
	for (unsigned int i = 0; i < planets_.size(); ++i) {
		const Planet& p = planets_[i];
		if (p.Owner() == 0) {
			r.push_back(p);
		}
	}
	return r;
}

std::vector<Planet> PlanetWars::EnemyPlanets() const {
	std::vector<Planet> r;
	for (unsigned int i = 0; i < planets_.size(); ++i) {
		const Planet& p = planets_[i];
		if (p.Owner() > 1) {
			r.push_back(p);
		}
	}
	return r;
}

std::vector<Planet> PlanetWars::NotMyPlanets() const {
	std::vector<Planet> r;
	for (unsigned int i = 0; i < planets_.size(); ++i) {
		const Planet& p = planets_[i];
		if (p.Owner() != 1) {
			r.push_back(p);
		}
	}
	return r;
}

std::vector<Fleet> PlanetWars::Fleets() const {
	std::vector<Fleet> r;
	for (unsigned int i = 0; i < fleets_.size(); ++i) {
		const Fleet& f = fleets_[i];
		r.push_back(f);
	}
	return r;
}

std::vector<Fleet> PlanetWars::MyFleets() const {
	std::vector<Fleet> r;
	for (unsigned int i = 0; i < fleets_.size(); ++i) {
		const Fleet& f = fleets_[i];
		if (f.Owner() == 1) {
			r.push_back(f);
		}
	}
	return r;
}

std::vector<Fleet> PlanetWars::EnemyFleets() const {
	std::vector<Fleet> r; 
	for (unsigned int i = 0; i < fleets_.size(); ++i) {
		const Fleet& f = fleets_[i];
		if (f.Owner() > 1) {
			r.push_back(f);
		}
	}
	return r;
}

void PlanetWars::AddFleet(Fleet const & new_fleet) {
	fleets_.push_back(new_fleet);
}

std::string PlanetWars::ToString() const {
	std::stringstream s;
	for (unsigned int i = 0; i < planets_.size(); ++i) {
		const Planet& p = planets_[i];
		s << "P " << p.X() << " " << p.Y() << " " << p.Owner()
			<< " " << p.NumShips() << " " << p.GrowthRate() << std::endl;
	}
	for (unsigned int i = 0; i < fleets_.size(); ++i) {
		const Fleet& f = fleets_[i];
		s << "F " << f.Owner() << " " << f.NumShips() << " "
			<< f.SourcePlanet() << " " << f.DestinationPlanet() << " "
			<< f.TotalTripLength() << " " << f.TurnsRemaining() << std::endl;
	}
	return s.str();
}

int PlanetWars::Distance(Planet const & source_planet, Planet const & destination_planet) {
	double dx = source_planet.X() - destination_planet.X();
	double dy = source_planet.Y() - destination_planet.Y();
	return (int)ceil(sqrt(dx * dx + dy * dy));
}

void PlanetWars::IssueOrder(Planet & source_planet, Planet const & destination_planet, int num_ships) {
	if (source_planet.PlanetID() == destination_planet.PlanetID()) throw std::runtime_error("Attempted to send ships from a planet to itself");
	if (num_ships >= source_planet.NumShips()) throw std::runtime_error("Not Enough Ships to send");
	if (source_planet.Owner() != SELF) throw std::runtime_error("You don't own that planet");
	AddFleet(Fleet(SELF, num_ships, source_planet.PlanetID(), destination_planet.PlanetID(), Distance(source_planet, destination_planet), Distance(source_planet, destination_planet)));
	source_planet.RemoveShips(num_ships);
	std::cout << source_planet.PlanetID() << " " << destination_planet.PlanetID() << " " << num_ships << std::endl;
	std::cout.flush();
}

bool PlanetWars::IsAlive(int player_id) const {
	for (unsigned int i = 0; i < planets_.size(); ++i) {
		if (planets_[i].Owner() == player_id) {
			return true;
		}
	}
	for (unsigned int i = 0; i < fleets_.size(); ++i) {
		if (fleets_[i].Owner() == player_id) {
			return true;
		}
	}
	return false;
}

int PlanetWars::NumShips(int player_id) const {
	int num_ships = 0;
	for (unsigned int i = 0; i < planets_.size(); ++i) {
		if (planets_[i].Owner() == player_id) {
			num_ships += planets_[i].NumShips();
		}
	}
	for (unsigned int i = 0; i < fleets_.size(); ++i) {
		if (fleets_[i].Owner() == player_id) {
			num_ships += fleets_[i].NumShips();
		}
	}
	return num_ships;
}

int PlanetWars::ParseGameState(const std::string& s) {
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
				return 0;
			}
			Planet p(planet_id++,              // The ID of this planet
				atoi(tokens[3].c_str()),  // Owner
				atoi(tokens[4].c_str()),  // Num ships
				atoi(tokens[5].c_str()),  // Growth rate
				atof(tokens[1].c_str()),  // X
				atof(tokens[2].c_str())); // Y
			planets_.push_back(p);
		} else if (tokens[0] == "F") {
			if (tokens.size() != 7) {
				return 0;
			}
			Fleet f(atoi(tokens[1].c_str()),  // Owner
				atoi(tokens[2].c_str()),  // Num ships
				atoi(tokens[3].c_str()),  // Source
				atoi(tokens[4].c_str()),  // Destination
				atoi(tokens[5].c_str()),  // Total trip length
				atoi(tokens[6].c_str())); // Turns remaining
			fleets_.push_back(f);
		} else {
			return 0;
		}
	}
	return 1;
}

void PlanetWars::FinishTurn() const {
	std::cout << "go" << std::endl;
	std::cout.flush();
}
