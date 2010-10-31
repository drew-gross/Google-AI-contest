#include "GameManager.h"
#include "Logger.h"
#include "Planet.h"
#include "StringUtil.h"

GameManager* GameManager::instance_ = nullptr;
Logger GameManager::mapdata("orders.txt");

int GameManager::Distance(Planet const * const source_planet, Planet const * const destination_planet) {
	double dx = source_planet->X() - destination_planet->X();
	double dy = source_planet->Y() - destination_planet->Y();
	return (int)ceil(sqrt(dx * dx + dy * dy));
}

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

void GameManager::IssueOrder(Planet * const source_planet, Planet const * const destination_planet, int num_ships) {
	if (source_planet->PlanetID() == destination_planet->PlanetID()) throw std::runtime_error("Attempted to send ships from a planet to itself");
	if (num_ships > source_planet->NumShips()) {
		throw std::runtime_error("Not Enough Ships to send");
	}
	if (source_planet->Owner() != Player::self()) throw std::runtime_error("You don't own that planet");
	if (num_ships == 0) return;

	state.AddFleet(new Fleet(Player::self(), num_ships, source_planet->PlanetID(), destination_planet->PlanetID(), Distance(source_planet, destination_planet), Distance(source_planet, destination_planet)));
	destination_planet->ClearFutureCache();
	source_planet->RemoveShips(num_ships);

	std::cout << source_planet->PlanetID() << " " << destination_planet->PlanetID() << " " << num_ships << std::endl;
	std::cout.flush();
}

void GameManager::Initialize() {
	if (instance_ != nullptr) {
		return;
	}
	instance_ = new GameManager();
	instance_->turn = 0;
}

GameManager& GameManager::Instance() {
	Initialize();
	return *instance_;
}
