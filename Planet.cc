#include "Planet.h"

#include "Utilities.h"

#include "PlanetWars.h"

Planet::Planet(int planet_id,
	int owner,
	int num_ships,
	int growth_rate,
	double x,
	double y) {
		planet_id_ = planet_id;
		owner_ = owner;
		num_ships_ = num_ships;
		growth_rate_ = growth_rate;
		x_ = x;
		y_ = y;
}

int Planet::PlanetID() const {
	return planet_id_;
}

int Planet::Owner() const {
	return owner_;
}

int Planet::NumShips() const {
	return num_ships_;
}

int Planet::NumShipsInTurns(int turns) const {
	unsigned int turnInFuture = 0;
	unsigned int shipsInTurnInFuture = NumShips();
	unsigned int ownerInTurnInFuture = Owner();
	for (int turnInFuture = 0; turnInFuture < turns; ++turnInFuture) {
		unsigned int totalEnemyShipsAttacking = 0;
		unsigned int totalPlayerShipsAttacking = 0;
		for (unsigned int i = 0; i < PlanetWars::Instance().Fleets().size(); ++i) {
			if (PlanetWars::Instance().Fleets()[i].Owner() == SELF) {
				totalPlayerShipsAttacking += PlanetWars::Instance().Fleets()[i].NumShips();
			}
			if (PlanetWars::Instance().Fleets()[i].Owner() == ENEMY) {
				totalEnemyShipsAttacking += PlanetWars::Instance().Fleets()[i].NumShips();
			}
		}
		switch (ownerInTurnInFuture) {
		case NEUTRAL:
			if (totalPlayerShipsAttacking > shipsInTurnInFuture && totalPlayerShipsAttacking > totalEnemyShipsAttacking) {
				ownerInTurnInFuture = SELF;
				shipsInTurnInFuture = totalPlayerShipsAttacking - std::max(shipsInTurnInFuture, totalEnemyShipsAttacking);
			}
			if (totalEnemyShipsAttacking > shipsInTurnInFuture && totalEnemyShipsAttacking > totalPlayerShipsAttacking) {
				ownerInTurnInFuture = ENEMY;
				shipsInTurnInFuture = totalEnemyShipsAttacking - std::max(shipsInTurnInFuture, totalPlayerShipsAttacking);
			}
			if (shipsInTurnInFuture > totalPlayerShipsAttacking && shipsInTurnInFuture > totalEnemyShipsAttacking) {
				ownerInTurnInFuture = NEUTRAL;
				shipsInTurnInFuture = shipsInTurnInFuture - std::max(totalEnemyShipsAttacking, totalPlayerShipsAttacking);
			}
			break;
		case SELF: 
			shipsInTurnInFuture += GrowthRate();
			shipsInTurnInFuture += totalPlayerShipsAttacking;
			if (totalEnemyShipsAttacking > shipsInTurnInFuture) {
				ownerInTurnInFuture = ENEMY;
				shipsInTurnInFuture = totalEnemyShipsAttacking - shipsInTurnInFuture;
			} else {
				shipsInTurnInFuture -= totalEnemyShipsAttacking;
			}
			break;
		case ENEMY:
			shipsInTurnInFuture += GrowthRate();
			shipsInTurnInFuture += totalEnemyShipsAttacking;
			if (totalPlayerShipsAttacking > shipsInTurnInFuture) {
				ownerInTurnInFuture = SELF;
				shipsInTurnInFuture = totalPlayerShipsAttacking - shipsInTurnInFuture;
			} else {
				shipsInTurnInFuture -= totalPlayerShipsAttacking;
			}
			break;
		}
	}
	return shipsInTurnInFuture;
}

int Planet::GrowthRate() const {
	return growth_rate_;
}

double Planet::X() const {
	return x_;
}

double Planet::Y() const {
	return y_;
}

void Planet::Owner(int new_owner) {
	owner_ = new_owner;
}

void Planet::NumShips(int new_num_ships) {
	num_ships_ = new_num_ships;
}

void Planet::AddShips(int amount) {
	num_ships_ += amount;
}

void Planet::RemoveShips(int amount) {
	num_ships_ -= amount;
}
