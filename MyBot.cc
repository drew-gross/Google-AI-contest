#include <iostream>
#include <exception>
#include <stdexcept>

#include "PlanetWars.h"
#include "Logger.h"

#include "DontNeedToAttackException.h"
#include "DontNeedToDefendException.h"

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
	//Defending phase
	PlanetList needToDefend = PlanetWars::Instance().Planets();
	for (unsigned int i = 0; i < needToDefend.size(); ++i) {
		try {
			if (needToDefend[i]->NeedToDefend()) {
				int optimalDefenseTime = needToDefend[i]->OptimalDefenseTime();
				PlanetList defendersAtOptimalTime;
				PlanetList defendersAfterOptimalTime;
				PlanetList defendersBeforeOptimalTime;

				PlanetList myPlanets = PlanetWars::Instance().PlanetsOwnedBy(self);
				for (unsigned int j = 0; j < myPlanets.size(); ++j) {
					int defenderDefendeeDistance = PlanetWars::Distance(*myPlanets[j], *needToDefend[i]);
					if (defenderDefendeeDistance == optimalDefenseTime) {
						defendersAtOptimalTime.push_back(myPlanets[j]);
					} else if (defenderDefendeeDistance < optimalDefenseTime) {
						defendersBeforeOptimalTime.push_back(myPlanets[j]);
					} else if (defenderDefendeeDistance > optimalDefenseTime) {
						defendersAfterOptimalTime.push_back(myPlanets[j]);
					}
				}
				for (unsigned int j = 0; j < defendersAtOptimalTime.size(); ++j) {
					if (!defendersAtOptimalTime[j]->NeedToDefend() && needToDefend[i]->NeedToDefend()) {
						PlanetWars::Instance().IssueOrder(*defendersAtOptimalTime[j], *needToDefend[i], std::max(defendersAtOptimalTime[j]->NumShips() - 1, needToDefend[i]->NumShipsInTurns(optimalDefenseTime +1)));
					}
				}
			}
		} catch (DontNeedToDefendException e) {
			//simply ignore, the for loop will continue to the next planet that needs defense anyway.
		}
	}

	// Attacking phase
	bool enoughShipsToAttack = true;
	PlanetList needToAttack = PlanetWars::Instance().Planets();
	while (enoughShipsToAttack) {
		Planet* source = PlanetWars::Instance().PlanetsOwnedBy(self).Strongest();
		int shipsToSend = 0;

		PlanetList attackFromHere = needToAttack;
		while (enoughShipsToAttack) {
			Planet* dest = attackFromHere.WeakestFromPlanet(*source);
			int sourceDestSeparation = PlanetWars::Distance(*source, *dest);
			shipsToSend = dest->NumShipsInTurns(sourceDestSeparation) + 1;

			if ((source != nullptr) && (dest != nullptr) && (source->NumShips() > shipsToSend)) {
				try {
					if (dest->OptimalAttackTime() <= sourceDestSeparation) {
						PlanetWars::Instance().IssueOrder(*source, *dest, shipsToSend);
					} else {
						attackFromHere.erase(std::remove(attackFromHere.begin(), attackFromHere.end(), dest));
					}
				} catch (DontNeedToAttackException e) {
					needToAttack.erase(std::remove(needToAttack.begin(), needToAttack.end(), e.GetPlanet()));
					attackFromHere.erase(std::remove(attackFromHere.begin(), attackFromHere.end(), e.GetPlanet()));
				}
			} else {
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
