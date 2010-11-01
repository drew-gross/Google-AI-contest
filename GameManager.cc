#include "GameManager.h"
#include "Logger.h"
#include "Planet.h"
#include "StringUtil.h"

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

int GameManager::ParseGameState(const std::string& s) {
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
				Player(atoi(tokens[3].c_str())),  // Owner
				atoi(tokens[4].c_str()),  // Num ships
				atoi(tokens[5].c_str()),  // Growth rate
				atof(tokens[1].c_str()),  // X
				atof(tokens[2].c_str())); // Y
			state.AddPlanet(p);
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
			state.AddFleet(f);
		} else {
			throw std::runtime_error("error parsing gamestate");
		}
	}
	return 1;
}

GameState& GameManager::State()
{
	return state;
}

GameManager::GameManager() {
}

void GameManager::IssueOrder(Planet * const source, Planet const * const dest, int num_ships) {
	if (source->PlanetID() == dest->PlanetID()) throw std::runtime_error("Attempted to send ships from a planet to itself");
	if (num_ships > source->NumShips()) {
		throw std::runtime_error("Not Enough Ships to send");
	}
	if (source->Owner() != Player::self()) throw std::runtime_error("You don't own that planet");
	if (num_ships == 0) return;

	state.AddFleet(new Fleet(Player::self(), num_ships, source->PlanetID(), dest->PlanetID(), GameState::Distance(source, dest), GameState::Distance(source, dest)));
	dest->ClearFutureCache();
	source->RemoveShips(num_ships);

	std::cout << source->PlanetID() << " " << dest->PlanetID() << " " << num_ships << std::endl;
	std::cout.flush();
}
