#include "PlanetState.h"

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
