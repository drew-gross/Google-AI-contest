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

Logger testLog = Logger("testing.txt");

void DoTurn() {
	testLog.Log("step 1");
	// (1) If we currently have a fleet in flight, just do nothing.
	if (PlanetWars::Instance().MyFleets().size() >= 1) {
		return;
	}
	testLog.Log("step 2");
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
	testLog.Log("step 3");
	// (3) Find the weakest enemy or neutral planet.
	int dest = -1;
	double dest_score = -999999.0;
	std::vector<Planet> not_my_planets = PlanetWars::Instance().NotMyPlanets();
	for (unsigned int i = 0; i < not_my_planets.size(); ++i) {
		const Planet& p = not_my_planets[i];
		double score = 1.0 / (1 + p.NumShips());
		if (score > dest_score) {
			dest_score = score;
			dest = p.PlanetID();
		}
	}
	testLog.Log("step 4");
	// (4) Send half the ships from my strongest planet to the weakest
	// planet that I do not own.
	if (source >= 0 && dest >= 0) {
		int num_ships = source_num_ships / 2;
		PlanetWars::Instance().IssueOrder(source, dest, num_ships);
	}
}

// This is just the main game loop that takes care of communicating with the
// game engine for you. You don't have to understand or change the code below.
int main(int argc, char *argv[]) {
	std::string current_line;
	std::string map_data;
	while (true) {
		int c = std::cin.get();
		current_line += (char)c;
		if (c == '\n') {
			if (current_line.length() >= 2 && current_line.substr(0, 2) == "go") {
				testLog.Log("Initializing");
				PlanetWars::Initialize(map_data);
				map_data = "";
				testLog.Log("Doing Turn");
				try {
					DoTurn();
				} catch (std::runtime_error e) {
					testLog.Log(e.what());
				}
				testLog.Log("Finishing Turn");
				PlanetWars::Instance().FinishTurn();
				testLog.Log("Uninitializing");
				PlanetWars::Uninitialize();
			} else {
				map_data += current_line;
			}
			current_line = "";
		}
	}
	return 0;
}
