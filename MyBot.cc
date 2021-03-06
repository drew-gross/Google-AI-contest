#include <iostream>
#include <exception>
#include <stdexcept>

#include "GameState.h"
#include "Logger.h"

#include "DontNeedToAttackException.h"
#include "DontNeedToDefendException.h"
#include "AI.h"
#include "GameManager.h"

// This is just the main game loop that takes care of communicating with the
// game engine for you. You don't have to understand or change the code below.
int main() {
	std::string current_line;
	std::string map_data;
	for (;;) {
		int c = std::cin.get();
		current_line += (char)c;
		if (c == '\n') {
			if (current_line.length() >= 2 && current_line.substr(0, 2) == "go") {
				GameManager::Instance().ParseGameState(map_data);
				GameManager::Instance().NextTurn();
				map_data = "";
			} else {
				map_data += current_line;
			}
			current_line = "";
		}
	}
}
