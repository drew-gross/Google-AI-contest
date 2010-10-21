#include <iostream>
#include "PlanetWars.h"
#include "Logger.h"

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
	Planet* source = PlanetWars::Instance().MyPlanets().Strongest();
	Planet* dest = PlanetWars::Instance().NotMyPlanets().Weakest();
	int shipsToSend = dest->NumShipsInTurns(PlanetWars::Distance(*source, *dest)) + 1;
	Player ownerAtArrival = dest->OwnerInTurns(PlanetWars::Distance(*source, *dest));
	if (source != nullptr && dest != nullptr && source->NumShips() > shipsToSend && ownerAtArrival != self) {
		PlanetWars::Instance().IssueOrder(*source, *dest, shipsToSend);
	}
}

// This is just the main game loop that takes care of communicating with the
// game engine for you. You don't have to understand or change the code below.
int main() {
	Logger exceptions = Logger("exceptions.txt");
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
