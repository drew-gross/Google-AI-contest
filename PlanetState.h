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

	PlanetState NextState(int totalPlayerShipsAttacking, int totalEnemyShipsAttacking, int growthRate );
	PlanetState AdvancementPhase(int growthRate );
	PlanetState ArrivalPhase(int playerAttackers, int enemyAttackers);
	PlanetState ResolveNeutralAttack(int playerAttackers, int enemyAttackers );
	PlanetState ResolveNonNeutralAttack(int playerAttackers, int enemyAttackers);

private:
	Player player;
	int ships;
};

#endif //PLANETSTATE_H_
