#include "PlanetState.h"
#include "Utilities.h"

Player PlanetState::GetPlayer() const
{
	return player;
}

PlanetState::PlanetState( Player newPlayer, int newShips ) : player(newPlayer), ships(newShips)
{

}

PlanetState::PlanetState()
{

}

int PlanetState::GetShips() const
{
	return ships;
}

void PlanetState::RemoveShips( int shipsSent )
{
	ships -= shipsSent;
}

void PlanetState::SetPlayer( Player newPlayer )
{
	player = newPlayer;
}

void PlanetState::SetShips( int newShips )
{
	ships = newShips;
	if (ships < 0) {
		ships *= -1;
		player = player.Opponent();
	}
}

PlanetState PlanetState::ArrivalPhase(int playerAttackers, int enemyAttackers )
{
	if (GetPlayer() == Player::neutral()) {
		return ResolveNeutralAttack(playerAttackers, enemyAttackers);
	} else {
		return ResolveNonNeutralAttack(playerAttackers, enemyAttackers);
	}
}

PlanetState PlanetState::ResolveNeutralAttack(int playerAttackers,  int enemyAttackers )
{
	PlanetState nextState;
	if (playerAttackers > GetShips() && playerAttackers > enemyAttackers) {
		nextState.SetPlayer(Player::self());
	}
	if (enemyAttackers > GetShips() && enemyAttackers > playerAttackers) {
		nextState.SetPlayer(Player::enemy());
	}
	if (GetShips() > playerAttackers && GetShips() > enemyAttackers) {
		nextState.SetPlayer(Player::neutral());
	}
	nextState.SetShips(std::max(GetShips(), playerAttackers, enemyAttackers) - std::median(GetShips(), playerAttackers, enemyAttackers));
	return nextState;
}

PlanetState PlanetState::ResolveNonNeutralAttack(int defenderShips, int attackerShips) {
	PlanetState nextState;
	nextState.SetPlayer(GetPlayer());
	nextState.SetShips(GetShips() + defenderShips - attackerShips);
	return nextState;
}

PlanetState PlanetState::NextState(int totalPlayerShipsAttacking, int totalEnemyShipsAttacking, int growthRate )
{
	return AdvancementPhase(growthRate).ArrivalPhase(totalPlayerShipsAttacking, totalEnemyShipsAttacking);
}

PlanetState PlanetState::AdvancementPhase(int growthRate )
{
	PlanetState nextState(*this);
	if (nextState.GetPlayer() != Player::neutral()) {
		nextState.SetShips(nextState.GetShips() + growthRate);
	}
	return nextState;
}
