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
#include <math.h>
#include <cmath>
#include "Utilities.h"
#include "NoPlanetsInListException.h"

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

Player Planet::OwnerInTurns(unsigned int turnsInFuture) const {
	return StateInTurns(turnsInFuture).GetPlayer();
}

int Planet::Ships() const {
	return CurrentState().GetShips();
}

int Planet::ShipsInTurns(unsigned int turnsInFuture) const {
	return StateInTurns(turnsInFuture).GetShips();
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
			continue;
		}
		if (futureStates[i].GetShips() < leastEnemyShips) {
			leastEnemyShips = futureStates[i].GetShips();
			leastEnemyShipsTime = i;
		}
	}
	return leastEnemyShipsTime;
}

bool Planet::NeedToDefend() const {
	std::vector<PlanetState> futureStates = FutureStates(GameManager::Instance().State().MaxDistance());
	if (futureStates[0].GetPlayer() != Player::self()) {
		return false;
	}
	for (unsigned int i = 0; i < futureStates.size(); ++i)
	{
		if (futureStates[i].GetPlayer() != Player::self())
		{
			return true;
		}
	}
	return false;
}

bool Planet::NeedToAttack() const
{
	return (OwnerInTurns(GameManager::Instance().State().MaxDistance()) != Player::self());
}

bool Planet::NeedToAttackCautiously() const
{
	if (OwnerInTurns(GameManager::Instance().State().MaxDistance()) == Player::enemy()) {
		return true;
	}
	if (OwnerInTurns(GameManager::Instance().State().MaxDistance()) == Player::neutral() && Ships() <= Player::self().Ships() - Player::enemy().Ships()) {
		return true;
	}
	return false;
}

void Planet::SeekDefenseFrom( PlanetList &defenders, int optimalDefenseTime) {
	for (PlanetList::iterator j = defenders.begin(); j != defenders.end(); ++j) {
		Planet * curDefender = *j;
		int defenseTime = std::max(DistanceTo(*j), optimalDefenseTime);
		if (*curDefender != *this) {
			GameManager::Instance().IssueOrder(curDefender, this, std::min(ShipsToTakeoverInTurns(defenseTime), curDefender->ShipsAvailable()));
		}
	}
}

int Planet::Growth() const {
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

int Planet::ShipsArrivingInTurns( Player fromPlayer, int turnsInFuture ) const
{
	int shipsArriving = 0;
	GameManager& gm = GameManager::Instance();
	for (unsigned int i = 0; i < gm.State().Fleets().size(); ++i)
	{
		Fleet * curFleet = gm.State().Fleets()[i];
		if (curFleet->ArrivesInTurns(turnsInFuture) && curFleet->DestinationPlanet() == this && curFleet->Owner() == fromPlayer) {
			shipsArriving += curFleet->Ships();
		}
	}
	return shipsArriving;
}

PlanetState Planet::StateInTurns(unsigned int turnsInFuture) const {
	if (stateInFuture.size() > turnsInFuture) {
		return stateInFuture[turnsInFuture];
	}
	if (stateInFuture.size() == 0) {
		stateInFuture.push_back(CurrentState());
	}
	unsigned int maxCachedTurnIndex = stateInFuture.size() - 1;
	PlanetState stateInTurn = stateInFuture.back();
	stateInFuture.resize(turnsInFuture + 2);

	for (unsigned int turnInFuture = maxCachedTurnIndex; turnInFuture <= turnsInFuture + 1; ++turnInFuture) {
		stateInFuture[turnInFuture] = stateInTurn;

		int totalEnemyShipsAttacking = ShipsArrivingInTurns(Player::enemy(), turnInFuture);
		int totalPlayerShipsAttacking = ShipsArrivingInTurns(Player::self(), turnInFuture);
		stateInTurn.NextState(totalPlayerShipsAttacking, totalEnemyShipsAttacking, Growth());
	}
	return stateInFuture[turnsInFuture];
}

int Planet::NeutralROI( int turns )
{
	return ((GameManager::Instance().TurnsRemaining() - turns) * Growth()) - ShipsInTurns(turns);
}

int Planet::EnemyROI( int turns )
{
	return (GameManager::Instance().TurnsRemaining() - turns) * 2 * Growth();
}

int Planet::ShipsAvailable()
{
	int shipsAvailable = Ships();
	for (int i = 0; i < GameManager::Instance().State().MaxDistance(); ++i)
	{
		shipsAvailable = std::min(shipsAvailable, MyShipsInTurns(i));
	}
	return shipsAvailable;
}

int Planet::PotentialAttackers() const
{
	try {
		Planet const * closestEnemy = ClosestPlanetInList(GameManager::Instance().State().Planets().OwnedBy(Player::enemy()));
		if (DistanceTo(closestEnemy) < DistanceTo(ClosestPlanetInList(GameManager::Instance().State().Planets().OwnedBy(Player::self())))) {
			return std::max(closestEnemy->Ships() - DistanceTo(closestEnemy) * Growth(), 0);
		} else {
			return 0;
		}
	} catch (NoPlanetsInListException) {
		return 0;
	}
}

int Planet::MyShipsInTurns( int turns )
{
	if (OwnerInTurns(turns) != Player::self()) {
		return 0;
	} else {
		return ShipsInTurns(turns);
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

Planet * Planet::ClosestPlanet()
{
	return ClosestPlanetInList(GameManager::Instance().State().Planets());
}

Planet * Planet::ClosestPlanetInList( PlanetList list ) const
{
	list.Remove(this);
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

int Planet::ShipsToTakeoverInTurns( unsigned int turns ) const
{
	return ShipsInTurns(turns) + 1;
}

PlanetState Planet::CurrentState() const
{
	return state;
}

std::vector<PlanetState> const & Planet::FutureStates( unsigned int turns) const
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
	return (int)std::ceil(std::sqrt(dx * dx + dy * dy));
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

void Planet::Reinforce( Planet const * p )
{
	GameManager::Instance().IssueOrder(this, p, ShipsAvailable());
}

void Planet::AttemptToTakeover( Planet const * p )
{
	GameManager::Instance().IssueOrder(this, p, std::min(ShipsAvailable(), p->ShipsToTakeoverInTurns(DistanceTo(p))));
}

bool Planet::CanTakeover( Planet const* p )
{
	return ShipsAvailable() >= p->ShipsToTakeoverInTurns(DistanceTo(p));
}

bool Planet::IsFront()
{
	Planet const * closestEnemy = ClosestPlanetOwnedBy(Player::enemy());
	return *(closestEnemy->ClosestPlanetOwnedBy(Player::self())) == *this;
}

bool Planet::AttackPlanets( PlanetList targets, PlanetList::Prioritiser attackFirst)
{
	bool attackSucceded = false;
	while (targets.size() > 0) {
		Planet * dest = (targets.*attackFirst)(this);
		int sourceDestSeparation = DistanceTo(dest);
		int destEnemySeparation;
		if (dest->Owner() == Player::neutral())
		{
			try {
				destEnemySeparation = dest->DistanceTo(dest->ClosestPlanetOwnedBy(Player::enemy()));
			} catch (NoPlanetsOwnedByPlayerException) {
				destEnemySeparation = std::numeric_limits<int>::max();
			}
		} else {
			destEnemySeparation = std::numeric_limits<int>::max();
		}

		if (dest != nullptr && (CanTakeover(dest)) && (sourceDestSeparation < destEnemySeparation)) {
			try {
				if (dest->OptimalAttackTime() <= sourceDestSeparation) {
					AttemptToTakeover(dest);
					attackSucceded = true;
				} else {
					targets.Remove(dest);
				}
			} catch (DontNeedToAttackException e) {
				targets.Remove(dest);
			}
		} else {
			targets.Remove(dest);
		}
	}
	return attackSucceded;
}
