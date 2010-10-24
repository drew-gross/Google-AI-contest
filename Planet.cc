#include "Planet.h"

#include <algorithm>

#include "Logger.h"

#include "PlanetWars.h"

#include "DontNeedToAttackException.h"
#include "DontNeedToDefendException.h"

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

int Planet::OptimalAttackTime() const {
	int optimalAttackTime = 0;
	int leastShips = std::numeric_limits<int>::max();
	for (int i = 0; i < PlanetWars::MaxDistance(); ++i) {
		if (OwnerInTurns(i) == self) {
			throw DontNeedToAttackException(this);
		}
		if (NumShipsInTurns(i) < leastShips) {
			optimalAttackTime = i;
			leastShips = NumShipsInTurns(i);
		}
	}
	return optimalAttackTime;
}

int Planet::OptimalDefenseTime() const {
	int optimalDefenseTime = 0;
	while (OwnerInTurns(optimalDefenseTime) != self) {
		optimalDefenseTime++;
		if (optimalDefenseTime > PlanetWars::MaxDistance()) {
			throw DontNeedToDefendException(this);
		}
	}
	while (OwnerInTurns(optimalDefenseTime) == self) {
		optimalDefenseTime++;
		if (optimalDefenseTime > PlanetWars::MaxDistance()) {
			throw DontNeedToDefendException(this);
		}
	}
	return optimalDefenseTime - 1;
}

bool Planet::NeedToDefend() const {
	return (OwnerInTurns(PlanetWars::MaxDistance()) != self);
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

void Planet::ClearFutureCache() const {
	stateInFuture.resize(0);
}

std::pair<int, Player> Planet::StateInTurns(unsigned int turns) const {
	if (stateInFuture.size() > turns) {
		return stateInFuture[turns];
	}
	std::pair<int, Player> stateInTurn;
	if (stateInFuture.size() == 0) {
		stateInTurn = std::pair<int, Player>(NumShips(), Owner());
		stateInFuture.push_back(stateInTurn);
	}
	unsigned int maxCachedTurnIndex = stateInFuture.size() - 1;
	stateInTurn = stateInFuture.back();
	stateInFuture.resize(turns + 1);

	for (unsigned int turnInFuture = maxCachedTurnIndex; turnInFuture <= turns; ++turnInFuture) {
		stateInFuture[turnInFuture] = stateInTurn;

		int totalEnemyShipsAttacking = 0;
		int totalPlayerShipsAttacking = 0;
		FleetList fleets = PlanetWars::Instance().Fleets();
		for (unsigned int i = 0; i < fleets.size(); ++i) {
			Fleet* curFleet = fleets[i];
			if (curFleet->ArrivesInTurns(turnInFuture) && (curFleet->DestinationPlanet() == this->PlanetID())) {
				if (curFleet->Owner() == self) {
					totalPlayerShipsAttacking += curFleet->NumShips();
				}
				if (curFleet->Owner() == enemy) {
					totalEnemyShipsAttacking += curFleet->NumShips();
				}
			}
		}
		if (stateInTurn.second != neutral) {
			stateInTurn.first += GrowthRate();
		}
		ResolveAttack(stateInTurn, totalPlayerShipsAttacking, totalEnemyShipsAttacking);
	}
	return stateInFuture[turns];
}

void Planet::ResolveAttack(std::pair<int, Player> & curState, int playerAttackers, int enemyAttackers) {
	if (curState.second == neutral) {
		if (playerAttackers > curState.first && playerAttackers > enemyAttackers) {
			curState.second = self;
		}
		if (enemyAttackers > curState.first && enemyAttackers > playerAttackers) {
			curState.second = enemy;
		}
		if (curState.first > playerAttackers && curState.first > enemyAttackers) {
			curState.second = neutral;
		}
		curState.first = std::max(curState.first, playerAttackers, enemyAttackers) - std::median(curState.first, playerAttackers, enemyAttackers);
		return;
	}
	ResolveNonNeutralAttack(curState, playerAttackers, enemyAttackers);
}

void Planet::ResolveNonNeutralAttack(std::pair<int, Player> & curState, int playerAttackers, int enemyAttackers) {
	if (curState.second == self) {
		curState.first = curState.first + playerAttackers - enemyAttackers;
		if (curState.first < 0) {
			curState.first *= -1;
			curState.second = enemy;
		}
		return;
	}
	if (curState.second == enemy) {
		curState.first = curState.first + enemyAttackers - playerAttackers;
		if (curState.first < 0) {
			curState.first *= -1;
			curState.second = self;
		}
		return;
	}
	throw std::logic_error("Attempted to resolve a non-neutral attack with a neutral planet involved!");
}
