#include "Force.h"
#include "GameState.h"
#include "GameManager.h"
#include <stdexcept>


Force::Force(Player owner,
	int num_ships,
	Planet const * destination_planet,
	unsigned int turns_remaining) : destination_planet_(destination_planet), 
									turns_remaining_(turns_remaining) {
	if (owner == Player::enemy())
	{
		enemyShips = num_ships;
		playerShips = 0;
	} else if (owner == Player::self())
	{
		playerShips = num_ships;
		enemyShips = 0;
	} else {
		throw std::logic_error("neutral can't send ships");
	}
}

Planet const * Force::DestinationPlanet() const {
	return destination_planet_;
}

unsigned int Force::TurnsRemaining() const {
	return turns_remaining_;
}

int Force::ShipsFromPlayer( Player player ) const
{
	if (player == Player::enemy()) {
		return enemyShips;
	} else if (player == Player::self())
	{
		return playerShips;
	}
	throw std::logic_error("Neutral can't send ships");
}

void Force::AddForce( Force const * const f )
{
	if (turns_remaining_ != f->TurnsRemaining())
	{
		throw std::logic_error("can't combine forces if they aren't arriving at the same time");
	}
	playerShips += f->PlayerShips();
	enemyShips += f->EnemyShips();
}

int Force::PlayerShips() const
{
	return playerShips;
}

int Force::EnemyShips() const
{
	return enemyShips;
}
