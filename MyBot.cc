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
	Logger mainLogger = Logger("main.txt");
	mainLogger.Enable();
	mainLogger.Log("Step 1");
	// (1) If we currently have a fleet in flight, just do nothing.
	if (PlanetWars::Instance().MyFleets().size() >= 1) {
		return;
	}
	mainLogger.Log("Step 2");
	// (2) Find my strongest planet.
	int source = -1;
	double source_score = -999999.0;
	int source_num_ships = 0;
	std::vector<Planet> my_planets = PlanetWars::Instance().MyPlanets();
	for (unsigned int i = 0; i < my_planets.size(); ++i) {
		const Planet& p = my_planets[i];
		double score = (double)p.NumShips();
		if (score > source_score) {
			source_score = score;
			source = p.PlanetID();
			source_num_ships = p.NumShips();
		}
	}
	mainLogger.Log("Step 3");
	// (3) Find the weakest enemy or neutral planet.
	int dest = -1;
	double dest_score = -999999.0;
	std::vector<Planet> not_my_planets = PlanetWars::Instance().NotMyPlanets();
	mainLogger.Log("Step 4");
	for (unsigned int i = 0; i < not_my_planets.size(); ++i) {
		const Planet& p = not_my_planets[i];
		mainLogger.Log("calculating ships in future");
		double score = 1.0 / (1 + p.NumShipsInTurns(PlanetWars::Instance().Distance(source, p.PlanetID())));
		mainLogger.Log("done calculating ships in future");
		if (score > dest_score) {
			dest_score = score;
			dest = p.PlanetID();
		}
	}
	mainLogger.Log("Step 5");
	if (source >= 0 && dest >= 0) {
		int num_ships = source_num_ships / 2;
		PlanetWars::Instance().IssueOrder(source, dest, PlanetWars::Instance().GetPlanet(dest).NumShipsInTurns(PlanetWars::Instance().Distance(source, dest)) + 1);
	}
}

// This is just the main game loop that takes care of communicating with the
// game engine for you. You don't have to understand or change the code below.
int main(int argc, char *argv[]) {
	Logger exceptions = Logger("exceptions.txt");
	exceptions.Enable();
	std::string current_line;
	std::string map_data;
	while (true) {
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
	return 0;
}
