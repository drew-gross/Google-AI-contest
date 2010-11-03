#include "Planet.h"

#include <algorithm>

#include "Logger.h"

#include "GameState.h"
#include "PlanetList.h"

#include "DontNeedToAttackException.h"
#include "DontNeedToDefendException.h"
#include "Player.h"
#include "GameManager.h"
#include "NoPlanetsOwnedByPlayerException.h"

Planet::Planet(int planet_id, PlanetState newstate, int growth_rate, double x, double y):
planet_id_(planet_id),
	state(newstate), 
	growth_rate_(growth_rate), 
	x_(x),
	y_(y)
{
}

int Planet::PlanetID() const {
	return planet_id_;
}

Player Planet::Owner() const {
	return CurrentState().GetPlayer();
}

Player Planet::OwnerInTurns(unsigned int turns) const {
	return StateInTurns(turns).GetPlayer();
}

int Planet::NumShips() const {
	return CurrentState().GetShips();
}

int Planet::NumShipsInTurns(unsigned int turns) const {
	return StateInTurns(turns).GetShips();
}

int Planet::OptimalAttackTime() const {
	int optimalAttackTime = 0;
	std::vector<PlanetState> futureStates = FutureStates(GameManager::Instance().State().MaxDistance());
	int leastShips = std::numeric_limits<int>::max();

	for (unsigned int i = 0; i < futureStates.size(); ++i) {
		if (futureStates[i].GetPlayer() == Player::self()) {
			throw DontNeedToAttackException(this);
		}
		if (futureStates[i].GetShips() < leastShips) {
			optimalAttackTime = i;
			leastShips = futureStates[i].GetShips();
		}
	}
	return optimalAttackTime;
}

int Planet::OptimalDefenseTime() const {
	int leastEnemyShipsTime = 0;
	int leastEnemyShips = std::numeric_limits<int>::max();
	std::vector<PlanetState> futureStates = FutureStates(GameManager::Instance().State().MaxDistance());
	for (unsigned int i = 0; i < futureStates.size(); ++i)
	{
		if (futureStates[i].GetPlayer() != Player::enemy())
		{
			break;
		}
		if (futureStates[i].GetShips() < leastEnemyShips) {
			leastEnemyShips = futureStates[i].GetShips();
			leastEnemyShipsTime = i;
		}
	}
	return leastEnemyShipsTime - 1;
}

bool Planet::NeedToDefend() const {
	bool returnval = false;
	std::vector<PlanetState> futureStates = FutureStates(GameManager::Instance().State().MaxDistance());
	for (unsigned int i = 0; i < futureStates.size(); ++i)
	{
		if (futureStates[i].GetPlayer() == Player::self())
		{
			returnval = true;
			break;
		}

	}
	return ((OwnerInTurns(GameManager::Instance().State().MaxDistance()) != Player::self()) && returnval);
}

bool Planet::NeedToAttack() const
{
	return (OwnerInTurns(GameManager::Instance().State().MaxDistance()) != Player::self());
}

void Planet::SeekDefenseFrom( PlanetList &defenders, int optimalDefenseTime) {
	for (PlanetList::iterator j = defenders.begin(); j != defenders.end(); ++j) {
		Planet * curDefender = *j;
		if (NeedToDefend() && curDefender->NumShipsAvailable() > 0) {
			GameManager::Instance().IssueOrder(curDefender, this, std::min(NumShipsInTurns(optimalDefenseTime + 1) + 1, curDefender->NumShipsAvailable()));
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

void Planet::RemoveShips(int amount) {
	state.RemoveShips(amount);
}

bool Planet::operator==(Planet const & rhs) const
{
	return PlanetID() == rhs.PlanetID();
}

bool Planet::operator!=( Planet const & rhs ) const
{
	return PlanetID() != rhs.PlanetID();
}

void Planet::ClearFutureCache() const {
	stateInFuture.clear();
}

int Planet::ShipsArrivingInTurns( Player fromPlayer, int numTurns ) const
{
	int shipsArriving = 0;
	GameManager& gm = GameManager::Instance();
	for (unsigned int i = 0; i < gm.State().Fleets().size(); ++i)
	{
		Fleet * curFleet = gm.State().Fleets()[i];
		if (curFleet->ArrivesInTurns(numTurns) && curFleet->DestinationPlanet() == this && curFleet->Owner() == fromPlayer) {
			shipsArriving += curFleet->NumShips();
		}
	}
	return shipsArriving;
}

PlanetState Planet::StateInTurns(unsigned int turns) const {
	if (stateInFuture.size() > turns) {
		return stateInFuture[turns];
	}
	if (stateInFuture.size() == 0) {
		stateInFuture.push_back(CurrentState());
	}
	unsigned int maxCachedTurnIndex = stateInFuture.size() - 1;
	PlanetState stateInTurn = stateInFuture.back();
	stateInFuture.resize(turns + 1);

	for (unsigned int turnInFuture = maxCachedTurnIndex; turnInFuture <= turns; ++turnInFuture) {
		stateInFuture[turnInFuture] = stateInTurn;

		int totalEnemyShipsAttacking = ShipsArrivingInTurns(Player::enemy(), turnInFuture);
		int totalPlayerShipsAttacking = ShipsArrivingInTurns(Player::self(), turnInFuture);
		stateInTurn = NextState(stateInTurn, totalPlayerShipsAttacking, totalEnemyShipsAttacking, GrowthRate());
	}
	return stateInFuture[turns];
}

PlanetState Planet::ArrivalPhase( PlanetState const& curState, int playerAttackers, int enemyAttackers )
{
	if (curState.GetPlayer() == Player::neutral()) {
		return ResolveNeutralAttack(curState, playerAttackers, enemyAttackers);
	} else {
		return ResolveNonNeutralAttack(curState, playerAttackers, enemyAttackers);
	}
}

PlanetState Planet::ResolveNeutralAttack(PlanetState const&curState, int playerAttackers,  int enemyAttackers )
{
	PlanetState nextState;
	if (playerAttackers > curState.GetShips() && playerAttackers > enemyAttackers) {
		nextState.SetPlayer(Player::self());
	}
	if (enemyAttackers > curState.GetShips() && enemyAttackers > playerAttackers) {
		nextState.SetPlayer(Player::enemy());
	}
	if (curState.GetShips() > playerAttackers && curState.GetShips() > enemyAttackers) {
		nextState.SetPlayer(Player::neutral());
	}
	nextState.SetShips(std::max(curState.GetShips(), playerAttackers, enemyAttackers) - std::median(curState.GetShips(), playerAttackers, enemyAttackers));
	return nextState;
}

PlanetState Planet::ResolveNonNeutralAttack(PlanetState const& curState, int defenderShips, int attackerShips) {
	PlanetState nextState;
	nextState.SetPlayer(curState.GetPlayer());
	nextState.SetShips(curState.GetShips() + defenderShips - attackerShips);
	return nextState;
}

PlanetState Planet::NextState( PlanetState const& stateInTurn, int totalPlayerShipsAttacking, int totalEnemyShipsAttacking, int growthRate )
{
	return ArrivalPhase(AdvancementPhase(stateInTurn, growthRate), totalPlayerShipsAttacking, totalEnemyShipsAttacking);
}

int Planet::NeutralROI( int turns )
{
	return ((GameManager::Instance().TurnsRemaining() - turns) * GrowthRate()) - NumShipsInTurns(turns);
}

int Planet::EnemyROI( int turns )
{
	return (GameManager::Instance().TurnsRemaining() - turns) * 2 * GrowthRate();
}

int Planet::NumShipsAvailable()
{
	int shipsAvailable = NumShips();
	for (int i = 0; i <= GameManager::Instance().State().MaxDistance(); ++i)
	{
		if (OwnerInTurns(i) != Player::self()) {
			return 0;
		}
		shipsAvailable = std::min(shipsAvailable, NumShipsInTurns(i));
	}
	return shipsAvailable;
}

PlanetState Planet::AdvancementPhase( PlanetState const& curState, int growthRate )
{
	PlanetState nextState(curState);
	if (nextState.GetPlayer() != Player::neutral()) {
		nextState.SetShips(nextState.GetShips() + growthRate);
	}
	return nextState;
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

Planet * Planet::ClosestPlanet()
{
	return ClosestPlanetInList(GameManager::Instance().State().Planets());
}

Planet * Planet::ClosestPlanetInList( PlanetList list )
{
	list.erase(std::remove(list.begin(), list.end(), this), list.end());
	if (list.size() == 0) {
		throw NoPlanetsOwnedByPlayerException(Player::enemy());
	}
	Planet * closestPlanet = list[0];
	int closestDistance = std::numeric_limits<int>::max();
	for (unsigned int i = 0; i < list.size(); ++i)
	{
		if (DistanceTo(list[i]) < closestDistance && PlanetID() != closestPlanet->PlanetID()) {
			closestDistance = DistanceTo(closestPlanet);
			closestPlanet = list[i];
		}
	}
	return closestPlanet;
}

int Planet::NumShipsToTakeoverInTurns( unsigned int turns ) const
{
	return NumShipsInTurns(turns) + 1;
}

PlanetState Planet::CurrentState() const
{
	return state;
}

std::vector<PlanetState> const & Planet::FutureStates( unsigned int turns ) const
{
	if (stateInFuture.size() > turns)
	{
		return stateInFuture;
	}
	StateInTurns(turns);
	return stateInFuture;
}

bool Planet::IsSupplier()
{
	Planet const * closestAlly;
	Planet const * closestEnemy;
	Planet const * closestNeutral;

	int distanceToAlly;
	int distanceToEnemy;
	int distanceToNeutral;

	try {
		closestAlly = ClosestPlanetOwnedBy(Player::self());
		distanceToAlly = DistanceTo(closestAlly);
	} catch (NoPlanetsOwnedByPlayerException e) {
		distanceToAlly = std::numeric_limits<int>::max();
	}

	try {
		closestEnemy = ClosestPlanetOwnedBy(Player::enemy());
		distanceToEnemy = DistanceTo(closestEnemy);
	} catch (NoPlanetsOwnedByPlayerException e) {
		distanceToEnemy = std::numeric_limits<int>::max();
	}

	try {
		closestNeutral = ClosestPlanetOwnedBy(Player::neutral());
		distanceToNeutral = DistanceTo(closestNeutral);
	} catch (NoPlanetsOwnedByPlayerException e) {
		distanceToNeutral = std::numeric_limits<int>::max();
	}

	return (2*distanceToAlly < distanceToEnemy && distanceToAlly < distanceToNeutral);
}

int Planet::DistanceTo( Planet const * p ) const
{
	double dx = X() - p->X();
	double dy = Y() - p->Y();
	return (int)ceil(sqrt(dx * dx + dy * dy));
}

Planet const * Planet::ClosestPlanetOwnedBy( Player player ) const
{
	PlanetList playerPlanets = GameManager::Instance().State().Planets().OwnedBy(player);
	if (playerPlanets.size() == 0) {
		throw NoPlanetsOwnedByPlayerException(player);
	}
	if (playerPlanets.size() == 1) {
		if ((*this) != (*playerPlanets[0]))
		{
			return playerPlanets[0];
		} else {
			throw NoPlanetsOwnedByPlayerException(player);
		}
	}
	Planet const * closestPlanet = playerPlanets[0];
	if (*this == *closestPlanet)
	{
		if (playerPlanets.size() == 2) {
			closestPlanet = playerPlanets[1];
		} else {
			throw NoPlanetsOwnedByPlayerException(player);
		}
	}
	for (unsigned int i = 1; i < playerPlanets.size(); ++i) {
		if (DistanceTo(closestPlanet) > DistanceTo(playerPlanets[i]) && (*this) != (*playerPlanets[i])) {
			closestPlanet = playerPlanets[i];
		}
	}
	return closestPlanet;
}
