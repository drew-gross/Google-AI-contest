#include "GameManager.h"
#include "Logger.h"
#include "Planet.h"
#include "StringUtil.h"
#include <stdexcept>

GameManager* GameManager::instance_ = nullptr;
Logger GameManager::mapdata("orders.txt");

int GameManager::MaxTurns()
{
	return 200;
}

int GameManager::Turns()
{
	return turn;
}

int GameManager::TurnsRemaining()
{
	return MaxTurns() - Turns();
}

void GameManager::NextTurn()
{
	turn++;
	ai.DoTurn();
	ai.FinishTurn();
}

void GameManager::ParseGameState( const std::string& s )
{
	mapdata.LogMapData(s);
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
				PlanetState(Player(atoi(tokens[3].c_str())), atoi(tokens[4].c_str())),  // Num ships
				atoi(tokens[5].c_str()),  // Growth rate
				atof(tokens[1].c_str()),  // X
				atof(tokens[2].c_str())); // Y
			state.AddPlanet(p);
		} else if (tokens[0] == "F") {
			if (tokens.size() != 7) {
				throw std::runtime_error("error parsing gamestate");
			}
			Force* f = new Force(atoi(tokens[1].c_str()),  // Owner
				atoi(tokens[2].c_str()),  // Num ships
				GameManager::Instance().State().GetPlanet(atoi(tokens[4].c_str())),  // Destination
				atoi(tokens[6].c_str())); // Turns remaining
			state.AddForce(f);
		} else {
			throw std::runtime_error("error parsing gamestate");
		}
	}
}

GameState& GameManager::State()
{
	return state;
}

GameManager::GameManager() {
}

void GameManager::IssueOrder(Planet * const source, Planet const * const dest, int ships) {
	if (source->PlanetID() == dest->PlanetID()) {
		throw std::runtime_error("Attempted to send ships from a planet to itself");
	}
	if (ships > source->Ships()) {
		throw std::runtime_error("Not Enough Ships to send");
	}
	if (ships < 0) {
		throw std::runtime_error("Attempting to send negative ships");
	}
	if (source->Owner() != Player::self()) throw std::runtime_error("You don't own that planet");
	if (ships == 0) {
		return;
	}

	state.AddForce(new Force(Player::self(), ships, dest, source->DistanceTo(dest)));
	source->RemoveShips(ships);

	std::cout << source->PlanetID() << " " << dest->PlanetID() << " " << ships << std::endl;
	std::cout.flush();
}
