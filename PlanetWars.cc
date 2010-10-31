#include "PlanetWars.h"
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

PlanetWars* PlanetWars::instance_ = nullptr;
Logger PlanetWars::mapdata("orders.txt");
Logger PlanetWars::uncaughtExceptions("exceptions.txt");

void PlanetWars::Initialize() {
	if (instance_ != nullptr) {
		return;
	}
	instance_ = new PlanetWars();
	instance_->turn = 0;
}

PlanetWars& PlanetWars::Instance() {
	Initialize();
	return *instance_;
}

PlanetWars::PlanetWars() {
}

int PlanetWars::NumPlanets() const {
	return planets_.size();
}

Planet const * PlanetWars::GetPlanet(int planet_id) {
	return planets_[planet_id];
}

int PlanetWars::NumFleets() const {
	return fleets_.size();
}

Fleet const * PlanetWars::GetFleet(int fleet_id) const {
	return fleets_[fleet_id];
}

PlanetList const & PlanetWars::Planets() const {
	return planets_;
}

void PlanetWars::AddFleet(Fleet* new_fleet) {
	fleets_.push_back(new_fleet);
}

std::string PlanetWars::ToString() const {
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

int PlanetWars::Distance(Planet const * const source_planet, Planet const * const destination_planet) {
	double dx = source_planet->X() - destination_planet->X();
	double dy = source_planet->Y() - destination_planet->Y();
	return (int)ceil(sqrt(dx * dx + dy * dy));
}

int PlanetWars::MaxDistance() {
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

void PlanetWars::IssueOrder(Planet * const source_planet, Planet const * const destination_planet, int num_ships) {
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

bool PlanetWars::IsAlive(int player_id) const {
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

int PlanetWars::NumShips(int player_id) const {
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

int PlanetWars::ParseGameState(const std::string& s) {
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

void PlanetWars::FinishTurn() const {
	instance_->planets_.DeleteAll();
	instance_->fleets_.DeleteAll();
	std::cout << "go" << std::endl;
	std::cout.flush();
}

int PlanetWars::MaxTurns()
{
	return 200;
}

int PlanetWars::Turns()
{
	return turn;
}

int PlanetWars::TurnsRemaining()
{
	return MaxTurns() - Turns();
}

void PlanetWars::DoTurn() {
	turn++;
	try
	{
		DefensePhase();
		AttackPhase();
		SupplyPhase();
	}
	catch (std::exception e)
	{
		uncaughtExceptions.Log(e.what());
	}
}

void PlanetWars::DefensePhase() {
	PlanetList needToDefend = Planets().NeedDefending();
	for (unsigned int i = 0; i < needToDefend.size(); ++i) {
		int optimalDefenseTime = needToDefend[i]->OptimalDefenseTime();
		PlanetList defendersAtOptimalTime;
		PlanetList defendersAfterOptimalTime;
		PlanetList defendersBeforeOptimalTime;

		PlanetList myPlanets = Planets().OwnedBy(Player::self());
		for (unsigned int j = 0; j < myPlanets.size(); ++j) {
			int defenderDefendeeDistance = Distance(myPlanets[j], needToDefend[i]);
			if (defenderDefendeeDistance == optimalDefenseTime) {
				defendersAtOptimalTime.push_back(myPlanets[j]);
			} else if (defenderDefendeeDistance < optimalDefenseTime) {
				defendersBeforeOptimalTime.push_back(myPlanets[j]);
			} else if (defenderDefendeeDistance > optimalDefenseTime) {
				defendersAfterOptimalTime.push_back(myPlanets[j]);
			}
		}
		try {
			needToDefend[i]->SeekDefenseFrom(defendersAtOptimalTime, optimalDefenseTime);
			if (defendersBeforeOptimalTime.NumShipsAvailable() > needToDefend[i]->NumShipsInTurns(optimalDefenseTime + 1)) {
				needToDefend[i]->SeekDefenseFrom(defendersBeforeOptimalTime, optimalDefenseTime);
				needToDefend[i]->SeekDefenseFrom(defendersAfterOptimalTime, optimalDefenseTime);
			}
		} catch (DontNeedToDefendException) {
			//do nothing - continue to next planet that needs defending
		}
	}
}

void PlanetWars::AttackPhase()
{
	bool shipsSent = true;

	PlanetList needToAttack = Planets().NeedAttacking();
	while (needToAttack.size() > 0 && shipsSent) {
		shipsSent = false;
		Planet* source = Planets().OwnedBy(Player::self()).Strongest();
		int shipsToSend = 0;

		PlanetList attackFromHere = needToAttack;
		while (attackFromHere.size() > 0) {
			Planet * dest = attackFromHere.HighestROIFromPlanet(source);
			int sourceDestSeparation = Distance(source, dest);
			shipsToSend = dest->NumShipsInTurns(sourceDestSeparation) + 1;

			if ((source != nullptr) && (dest != nullptr) && (source->NumShipsAvailable() >= shipsToSend)) {
				if (dest->OptimalAttackTime() <= sourceDestSeparation) {
					IssueOrder(source, dest, std::min(shipsToSend, source->NumShipsAvailable()));
					shipsSent = true;
				} else {
					attackFromHere.erase(std::remove(attackFromHere.begin(), attackFromHere.end(), dest), attackFromHere.end());
				}
			} else {
				attackFromHere.erase(std::remove(attackFromHere.begin(), attackFromHere.end(), dest), attackFromHere.end());
				needToAttack.erase(std::remove(needToAttack.begin(), needToAttack.end(), dest), needToAttack.end());
			}
		}
	}
}

void PlanetWars::UnitTest()
{
	//none yet
}

void PlanetWars::SupplyPhase()
{
	PlanetList fronts = Planets().Fronts();
	PlanetList myPlanets = Planets().OwnedBy(Player::self());
	for (unsigned int i = 0; i < myPlanets.size(); ++i)
	{
		if (myPlanets[i]->ClosestPlanetInList(fronts) != nullptr && myPlanets[i]->ClosestPlanetInList(fronts)->Owner() == Player::self())
		{
			PlanetWars::Instance().IssueOrder(myPlanets[i], myPlanets[i]->ClosestPlanetInList(fronts), std::min(myPlanets[i]->NumShipsAvailable(), myPlanets[i]->NumShips()));
		}	
	}
}
