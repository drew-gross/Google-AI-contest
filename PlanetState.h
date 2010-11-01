#ifndef PLANETSTATE_H_
#define PLANETSTATE_H_

#include "Player.h"

class PlanetState {
public:
	PlanetState(Player newPlayer, int newShips);
	PlanetState();

	Player GetPlayer() const;
	void SetPlayer(Player newPlayer);
	int GetShips() const;
	void SetShips(int newShips);

	void RemoveShips(int shipsSent);

private:
	Player player;
	int ships;
};

#endif //PLANETSTATE_H_
