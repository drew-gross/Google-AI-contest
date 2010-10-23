#include <iostream>
#include <exception>
#include <stdexcept>

#include "PlanetWars.h"
#include "Logger.h"

#include "DontNeedToAttackException.h"

// The DoTurn function is where your code goes. The PlanetWars object contains
// the state of the game, including information about all planets and fleets
// that currently exist. Inside this function, you issue orders using the
// pw.IssueOrder() function. For example, to send 10 ships from planet 3 to
// planet 8, you would say pw.IssueOrder(3, 8, 10).
//
// There is already a basic strategy in place here. You can use it as a
// starting point, or you can throw it out entirely and replace it with your
// own. Check out the tutorials and articles on the contest website at
// http://www.ai-contest.com/resources.

void DoTurn() {
	static Logger log("main.txt");
	bool enoughShipsToAttack = true;

	PlanetList needToAttack = PlanetWars::Instance().Planets();
	while (enoughShipsToAttack) {
		Planet* source = PlanetWars::Instance().PlanetsOwnedBy(self).Strongest();
		int shipsToSend = 0;
		while (enoughShipsToAttack) {
			PlanetList attackFromHere = needToAttack;
			Planet* dest = attackFromHere.WeakestFromPlanet(*source);
			int sourceDestSeparation = PlanetWars::Distance(*source, *dest);
			shipsToSend = dest->NumShipsInTurns(sourceDestSeparation) + 1;


			log.Log("Weakest of need to attack: ", dest->PlanetID());
			if ((source != nullptr) && (dest != nullptr) && (source->NumShips() > shipsToSend)) {
				try {
					if (dest->OptimalAttackTime() <= sourceDestSeparation) {
						log.Log("Attacking planet: ", dest->PlanetID());
						PlanetWars::Instance().IssueOrder(*source, *dest, shipsToSend);
					} else {
						log.Log("Removing planet that I don't need to attack: ", dest->PlanetID());
						attackFromHere.erase(std::remove(attackFromHere.begin(), attackFromHere.end(), dest));
						log.Log("Planets that need attacking: ", attackFromHere.size());
					}
				} catch (DontNeedToAttackException e) {
					log.Log("Removing planet that I don't need to attack: ", (&(e.getPlanet()))->PlanetID());
					needToAttack.erase(std::remove(needToAttack.begin(), needToAttack.end(), &(e.getPlanet())));
					attackFromHere.erase(std::remove(attackFromHere.begin(), attackFromHere.end(), &(e.getPlanet())));
					log.Log("Planets that need attacking: ", needToAttack.size());
				}
			} else {
				log.Log("Not enough ships to attack");
				enoughShipsToAttack = false;
			}
		}
	}
}

// This is just the main game loop that takes care of communicating with the
// game engine for you. You don't have to understand or change the code below.
int main() {
	Logger exceptions("exceptions.txt");
	std::string current_line;
	std::string map_data;
	for (;;) {
		int c = std::cin.get();
		current_line += (char)c;
		if (c == '\n') {
			if (current_line.length() >= 2 && current_line.substr(0, 2) == "go") {
				PlanetWars::Initialize(map_data);
				map_data = "";
				try {
					DoTurn();
				} catch (std::runtime_error e) {
					exceptions.Log(e.what());
				}
				PlanetWars::Instance().FinishTurn();
				PlanetWars::Uninitialize();
			} else {
				map_data += current_line;
			}
			current_line = "";
		}
	}
}
