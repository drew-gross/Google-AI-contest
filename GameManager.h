#ifndef GAMEMANAGER_H_
#define GAMEMANAGER_H_

#include "GameState.h"
#include "AI.h"

class GameManager {
public:
	static void Initialize();
	static GameManager& Instance();

	// Returns the total number of turns in a game
	int MaxTurns();

	// Returns the number of turns that have gone by
	int Turns();

	// Returns the number of turns remaining in the game
	int TurnsRemaining();

	void NextTurn();

	// Parses a game state from a string.
	int ParseGameState(const std::string& s);

	// Sends an order to the game engine. The order is to send num_ships ships
	// from source_planet to destination_planet. The order must be valid, or
	// else your bot will get kicked and lose the game. For example, you must own
	// source_planet, and you can't send more ships than you actually have on
	// that planet.
	void IssueOrder(Planet * const source_planet, Planet const * const destination_planet, int num_ships);

	GameState& State();

protected:
	// Initializes the game state given a string containing game state data.
	GameManager();
private:
	// The current game turn
	int turn;

	AI ai;
	GameState state;

	// Output log for sending orders to the engine.
	static Logger mapdata;

	// The GameState Singleton
	static GameManager* instance_;
};
#endif //GAMEMANAGER_H_
