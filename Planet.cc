#include "Planet.h"

#include "Utilities.h"
#include "Logger.h"

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

int Planet::NumShipsInTurns(unsigned int turns) const {
	static Logger numShipsInTurn = Logger("numShipsInTurn.txt");
	numShipsInTurn.Enable();
	numShipsInTurn.Log("entered function");
	unsigned int shipsInTurnInFuture = NumShips();
	unsigned int ownerInTurnInFuture = Owner();
	
	numShipsInTurn.Log("entering first for loop");
	for (unsigned int turnInFuture = 0; turnInFuture < turns; ++turnInFuture) {
		numShipsInTurn.Log("adding up attacker fleets");
		unsigned int totalEnemyShipsAttacking = 0;
		unsigned int totalPlayerShipsAttacking = 0;
		std::vector<Fleet> fleets = PlanetWars::Instance().Fleets();
		for (unsigned int i = 0; i < fleets.size(); ++i) {
			if (fleets[i].Owner() == SELF) {
				totalPlayerShipsAttacking += fleets[i].NumShips();
			}
			if (fleets[i].Owner() == ENEMY) {
				totalEnemyShipsAttacking += fleets[i].NumShips();
			}
		}

		numShipsInTurn.Log("calculating next turn");
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
