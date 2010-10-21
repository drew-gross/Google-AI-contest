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

int Planet::OwnerInTurns(unsigned int turns) const {
	int shipsInTurnInFuture = NumShips();
	unsigned int ownerInTurnInFuture = Owner();

	for (unsigned int turnInFuture = 0; turnInFuture < turns; ++turnInFuture) {
		int totalEnemyShipsAttacking = 0;
		int totalPlayerShipsAttacking = 0;
		std::vector<Fleet> fleets = PlanetWars::Instance().Fleets();
		for (unsigned int i = 0; i < fleets.size(); ++i) {
			Fleet const & curFleet = fleets[i];
			if (curFleet.ArrivesInTurns(turnInFuture) && (curFleet.DestinationPlanet() == this->PlanetID())) {
				if (curFleet.Owner() == SELF) {
					totalPlayerShipsAttacking += curFleet.NumShips();
				}
				if (curFleet.Owner() == ENEMY) {
					totalEnemyShipsAttacking += curFleet.NumShips();
				}
			}
		}

		switch (ownerInTurnInFuture) {
		case NEUTRAL:
			if (totalPlayerShipsAttacking > shipsInTurnInFuture && totalPlayerShipsAttacking > totalEnemyShipsAttacking) {
				ownerInTurnInFuture = SELF;
			}
			if (totalEnemyShipsAttacking > shipsInTurnInFuture && totalEnemyShipsAttacking > totalPlayerShipsAttacking) {
				ownerInTurnInFuture = ENEMY;
			}
			if (shipsInTurnInFuture > totalPlayerShipsAttacking && shipsInTurnInFuture > totalEnemyShipsAttacking) {
				ownerInTurnInFuture = NEUTRAL;
			}
			shipsInTurnInFuture = std::max(shipsInTurnInFuture, totalPlayerShipsAttacking, totalEnemyShipsAttacking) - std::median(shipsInTurnInFuture, totalPlayerShipsAttacking, totalEnemyShipsAttacking);
			break;

		case SELF: 
			shipsInTurnInFuture = shipsInTurnInFuture + GrowthRate() + totalPlayerShipsAttacking - totalEnemyShipsAttacking;
			if (shipsInTurnInFuture < 0) {
				shipsInTurnInFuture *= -1;
				ownerInTurnInFuture = ENEMY;
			}
			break;

		case ENEMY:
			shipsInTurnInFuture = shipsInTurnInFuture + GrowthRate() + totalEnemyShipsAttacking - totalPlayerShipsAttacking;
			if (shipsInTurnInFuture < 0) {
				shipsInTurnInFuture *= -1;
				ownerInTurnInFuture = SELF;
			}
			break;
		}
	}
	return ownerInTurnInFuture;
}

int Planet::Owner() const {
	return owner_;
}

int Planet::NumShips() const {
	return num_ships_;
}

int Planet::NumShipsInTurns(unsigned int turns) const {
	int shipsInTurnInFuture = NumShips();
	unsigned int ownerInTurnInFuture = Owner();

	for (unsigned int turnInFuture = 0; turnInFuture < turns; ++turnInFuture) {
		int totalEnemyShipsAttacking = 0;
		int totalPlayerShipsAttacking = 0;
		std::vector<Fleet> fleets = PlanetWars::Instance().Fleets();
		for (unsigned int i = 0; i < fleets.size(); ++i) {
			Fleet const & curFleet = fleets[i];
			if (curFleet.ArrivesInTurns(turnInFuture) && (curFleet.DestinationPlanet() == this->PlanetID())) {
				if (curFleet.Owner() == SELF) {
					totalPlayerShipsAttacking += curFleet.NumShips();
				}
				if (curFleet.Owner() == ENEMY) {
					totalEnemyShipsAttacking += curFleet.NumShips();
				}
			}
		}

		switch (ownerInTurnInFuture) {
		case NEUTRAL:
			if (totalPlayerShipsAttacking > shipsInTurnInFuture && totalPlayerShipsAttacking > totalEnemyShipsAttacking) {
				ownerInTurnInFuture = SELF;
			}
			if (totalEnemyShipsAttacking > shipsInTurnInFuture && totalEnemyShipsAttacking > totalPlayerShipsAttacking) {
				ownerInTurnInFuture = ENEMY;
			}
			if (shipsInTurnInFuture > totalPlayerShipsAttacking && shipsInTurnInFuture > totalEnemyShipsAttacking) {
				ownerInTurnInFuture = NEUTRAL;
			}
			shipsInTurnInFuture = std::max(shipsInTurnInFuture, totalPlayerShipsAttacking, totalEnemyShipsAttacking) - std::median(shipsInTurnInFuture, totalPlayerShipsAttacking, totalEnemyShipsAttacking);
			break;

		case SELF: 
			shipsInTurnInFuture = shipsInTurnInFuture + GrowthRate() + totalPlayerShipsAttacking - totalEnemyShipsAttacking;
			if (shipsInTurnInFuture < 0) {
				shipsInTurnInFuture *= -1;
				ownerInTurnInFuture = ENEMY;
			}
			break;

		case ENEMY:
			shipsInTurnInFuture = shipsInTurnInFuture + GrowthRate() + totalEnemyShipsAttacking - totalPlayerShipsAttacking;
			if (shipsInTurnInFuture < 0) {
				shipsInTurnInFuture *= -1;
				ownerInTurnInFuture = SELF;
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

bool Planet::operator==(Planet const & rhs) {
	return PlanetID() == rhs.PlanetID();
}