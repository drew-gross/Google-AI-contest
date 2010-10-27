#include "Planet.h"

#include <algorithm>

#include "Logger.h"

#include "PlanetWars.h"
#include "PlanetList.h"

#include "DontNeedToAttackException.h"
#include "DontNeedToDefendException.h"
#include "Player.h"

Planet::Planet(int planet_id, Player ownerNum, int num_ships, int growth_rate, double x, double y):
planet_id_(planet_id),
	owner_(ownerNum), 
	num_ships_(num_ships), 
	growth_rate_(growth_rate), 
	x_(x),
	y_(y)
{
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
	for (int i = 0; i < PlanetWars::Instance().MaxDistance(); ++i) {
		if (OwnerInTurns(i) == Player::self()) {
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
	while (OwnerInTurns(optimalDefenseTime) != Player::self()) {
		optimalDefenseTime++;
		if (optimalDefenseTime > PlanetWars::Instance().MaxDistance()) {
			throw DontNeedToDefendException(this);
		}
	}
	while (OwnerInTurns(optimalDefenseTime) == Player::self()) {
		optimalDefenseTime++;
		if (optimalDefenseTime > PlanetWars::Instance().MaxDistance()) {
			throw DontNeedToDefendException(this);
		}
	}
	return optimalDefenseTime - 1;
}

bool Planet::NeedToDefend() const {
	return (OwnerInTurns(PlanetWars::Instance().MaxDistance()) != Player::self());
}


void Planet::SeekDefenseFrom( PlanetList &defenders, int optimalDefenseTime) {
	for (unsigned int j = 0; j < defenders.size(); ++j) {
		Planet * curDefender = defenders[j];
		if (NeedToDefend() && curDefender->NumShipsAvailable() > 0) {
			PlanetWars::Instance().IssueOrder(*curDefender, *this, std::min(NumShipsInTurns(optimalDefenseTime + 1) + 1, curDefender->NumShipsAvailable()));
		}
	}
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
	stateInFuture.clear();
}

int Planet::ShipsArrivingInTurns( Player fromPlayer, int numTurns ) const
{
	int shipsArriving = 0;
	FleetList fleets = PlanetWars::Instance().Fleets(); 
	for (unsigned int i = 0; i < fleets.size(); ++i)
	{
		Fleet * curFleet = fleets[i];
		if (curFleet->ArrivesInTurns(numTurns) && curFleet->DestinationPlanet() == this && curFleet->Owner() == fromPlayer) {
			shipsArriving += curFleet->NumShips();
		}
	}
	return shipsArriving;
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

		int totalEnemyShipsAttacking = ShipsArrivingInTurns(Player::enemy(), turnInFuture);
		int totalPlayerShipsAttacking = ShipsArrivingInTurns(Player::self(), turnInFuture);
		NextState(stateInTurn, totalPlayerShipsAttacking, totalEnemyShipsAttacking, GrowthRate());
	}
	return stateInFuture[turns];
}

void Planet::ArrivalPhase(std::pair<int, Player> & curState, int playerAttackers, int enemyAttackers) {
	if (curState.second == Player::neutral()) {
		ResolveNeutralAttack(curState, playerAttackers, enemyAttackers);
	} else {
		ResolveNonNeutralAttack(curState, playerAttackers, enemyAttackers);
	}
}

void Planet::ResolveNeutralAttack(std::pair<int, Player> &curState, int playerAttackers,  int enemyAttackers )
{
	if (playerAttackers > curState.first && playerAttackers > enemyAttackers) {
		curState.second = Player::self();
	}
	if (enemyAttackers > curState.first && enemyAttackers > playerAttackers) {
		curState.second = Player::enemy();
	}
	if (curState.first > playerAttackers && curState.first > enemyAttackers) {
		curState.second = Player::neutral();
	}
	curState.first = std::max(curState.first, playerAttackers, enemyAttackers) - std::median(curState.first, playerAttackers, enemyAttackers);
}

void Planet::ResolveNonNeutralAttack(std::pair<int, Player> & curState, int defenderShips, int attackerShips) {
	curState.first = curState.first + defenderShips - attackerShips;
	if (curState.first < 0) {
		curState.first *= -1;
		curState.second = curState.second.Opponent();
	}
	return;
}

void Planet::NextState( std::pair<int, Player> &stateInTurn, int totalPlayerShipsAttacking, int totalEnemyShipsAttacking, int growthRate )
{
	AdvancementPhase(stateInTurn, growthRate);
	ArrivalPhase(stateInTurn, totalPlayerShipsAttacking, totalEnemyShipsAttacking);
}

int Planet::NeutralROI( int turns )
{
	CreateStaticLogger(numturns);
	numturns.LogVar(PlanetWars::Instance().TurnsRemaining());
	numturns.LogVar(turns);
	return ((PlanetWars::Instance().TurnsRemaining() - turns) * GrowthRate()) - NumShipsInTurns(turns);
}

int Planet::EnemyROI( int turns )
{
	return (PlanetWars::Instance().TurnsRemaining() - turns) * 2 * GrowthRate();
}

int Planet::NumShipsAvailable()
{
	if (NeedToDefend() == true) {
		return 0;
	}
	FleetList fleets = PlanetWars::Instance().Fleets();

	int shipsAvailable = NumShips();
	for (unsigned int i = 0; i < fleets.size(); ++i)
	{
		if (fleets[i]->DestinationPlanet() == this) {
			shipsAvailable = std::min(shipsAvailable, NumShipsInTurns(fleets[i]->TurnsRemaining()));
		}
	}
	return std::min(shipsAvailable, NumShips());
}

void Planet::AdvancementPhase( std::pair<int, Player> &stateInTurn, int growthRate )
{
	if (stateInTurn.second != Player::neutral()) {
		stateInTurn.first += growthRate;
	}
}

int Planet::ReturnOnInvestment( int turns )
{
	if (OwnerInTurns(turns) == Player::neutral()) {
		return NeutralROI(turns);
	} else if (OwnerInTurns(turns) == Player::enemy())
	{
		return EnemyROI(turns);
	} else
	{
		throw DontNeedToAttackException(this);
	}
}
