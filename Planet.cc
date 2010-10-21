#include "Planet.h"

#include "Utilities.h"
#include "Logger.h"

#include "PlanetWars.h"

Planet::Planet(int planet_id,
	Player owner,
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

Player Planet::Owner() const {
	return owner_;
}

Player Planet::OwnerInTurns(unsigned int turns) const {
	return StateInTurns(turns).second;
}

int Planet::NumShips() const {
	return num_ships_;
}

int Planet::NumShipsInTurns(unsigned int turns) const {
	return StateInTurns(turns).first;
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

void Planet::Owner(Player new_owner) {
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

std::pair<int, Player> Planet::StateInTurns(unsigned int turns) const {
	std::pair<int, Player> stateInTurn(NumShips(), Owner());

	for (unsigned int turnInFuture = 0; turnInFuture < turns; ++turnInFuture) {
		int totalEnemyShipsAttacking = 0;
		int totalPlayerShipsAttacking = 0;
		FleetList fleets = PlanetWars::Instance().Fleets();
		for (unsigned int i = 0; i < fleets.size(); ++i) {
			Fleet const & curFleet = fleets[i];
			if (curFleet.ArrivesInTurns(turnInFuture) && (curFleet.DestinationPlanet() == this->PlanetID())) {
				if (curFleet.Owner() == self) {
					totalPlayerShipsAttacking += curFleet.NumShips();
				}
				if (curFleet.Owner() == enemy) {
					totalEnemyShipsAttacking += curFleet.NumShips();
				}
			}
		}

		switch (stateInTurn.second) {
		case neutral:
			if (totalPlayerShipsAttacking > stateInTurn.first && totalPlayerShipsAttacking > totalEnemyShipsAttacking) {
				stateInTurn.second = self;
			}
			if (totalEnemyShipsAttacking > stateInTurn.first && totalEnemyShipsAttacking > totalPlayerShipsAttacking) {
				stateInTurn.second = enemy;
			}
			if (stateInTurn.first > totalPlayerShipsAttacking && stateInTurn.first > totalEnemyShipsAttacking) {
				stateInTurn.second = neutral;
			}
			stateInTurn.first = std::max(stateInTurn.first, totalPlayerShipsAttacking, totalEnemyShipsAttacking) - std::median(stateInTurn.first, totalPlayerShipsAttacking, totalEnemyShipsAttacking);
			break;

		case self: 
			stateInTurn.first = stateInTurn.first + GrowthRate() + totalPlayerShipsAttacking - totalEnemyShipsAttacking;
			if (stateInTurn.first < 0) {
				stateInTurn.first *= -1;
				stateInTurn.second = enemy;
			}
			break;

		case enemy:
			stateInTurn.first = stateInTurn.first + GrowthRate() + totalEnemyShipsAttacking - totalPlayerShipsAttacking;
			if (stateInTurn.first < 0) {
				stateInTurn.first *= -1;
				stateInTurn.second = self;
			}
			break;
		}
	}
	return stateInTurn;
}
