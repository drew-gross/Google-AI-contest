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
	reservedShips = 0;
	canSave = true;
}

bool Planet::IsSaveable() const
{
	return canSave;
}

int Planet::PlanetID() const {
	return planet_id_;
}

Player Planet::Owner() const {
	return CurrentState().GetPlayer();
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

bool Planet::NeedToDefend( void ) const
{
	std::vector<PlanetState> futureStates = FutureStates(GameManager::Instance().State().MaxDistance());
	if (Owner() != Player::self()) {
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
			curDefender->SendShips(this, std::min(ShipsToTakeoverInTurns(defenseTime), curDefender->ShipsAvailable() - curDefender->PotentialAttackers()));
			curDefender->PotentialAttacker()->ReserveShips(curDefender->PotentialAttackers());
		}
	}
}

void Planet::ReserveDefenseFrom( PlanetList &defenders, int optimalDefenseTime) {
	for (PlanetList::iterator j = defenders.begin(); j != defenders.end(); ++j) {
		Planet * curDefender = *j;
		int defenseTime = std::max(DistanceTo(*j), optimalDefenseTime);
		if (*curDefender != *this) {
			curDefender->ReserveShips(std::min(ShipsToTakeoverInTurns(defenseTime), curDefender->ShipsAvailable() - curDefender->PotentialAttackers()));
			curDefender->PotentialAttacker()->ReserveShips(curDefender->PotentialAttackers());
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
	futureStates.clear();
}

int Planet::ShipsArrivingInTurns( Player fromPlayer, int turnsInFuture ) const
{
	int shipsArriving = 0;
	FleetList const & fleets = GameManager::Instance().State().Forces();
	for (unsigned int i = 0; i < fleets.size(); ++i)
	{
		Force * curFleet = fleets[i];
		if (curFleet->DestinationPlanet() == this && curFleet->ArrivesInTurns(turnsInFuture)) {
			shipsArriving += curFleet->ShipsFromPlayer(fromPlayer);
		}
	}
	return shipsArriving;
}

PlanetState Planet::StateInTurns(unsigned int turnsInFuture) const {
	if (turnsInFuture < futureStates.size())
	{
		return futureStates[turnsInFuture];
	}
	futureStates.clear();
	futureStates.push_back(CurrentState());
	futureStates.resize(turnsInFuture + 1);
	PlanetState stateInTurn = CurrentState();
	for (unsigned int turnInFuture = 0; turnInFuture < futureStates.size(); ++turnInFuture) {
		futureStates[turnInFuture] = stateInTurn;

		int totalEnemyShipsAttacking = ShipsArrivingInTurns(Player::enemy(), turnInFuture);
		int totalPlayerShipsAttacking = ShipsArrivingInTurns(Player::self(), turnInFuture);
		stateInTurn.NextState(totalPlayerShipsAttacking, totalEnemyShipsAttacking, Growth());
	}
	return futureStates[turnsInFuture];
}

int Planet::NeutralROI( int turns )
{
	return ((GameManager::Instance().TurnsRemaining() - turns) * Growth()) - ShipsInTurns(turns);
}

int Planet::EnemyROI( int turns )
{
	return (GameManager::Instance().TurnsRemaining() - turns) * 2 * Growth();
}

int Planet::ShipsAvailable() const
{
	if (!IsSaveable()) {
		return Ships();
	}
	int shipsAvailable = Ships();
	for (int i = 0; i < GameManager::Instance().State().MaxDistance(); ++i)
	{
		shipsAvailable = std::min(shipsAvailable, MyShipsInTurns(i));
	}
	return shipsAvailable - reservedShips;
}

Planet * Planet::PotentialAttacker() const {
	return ClosestPlanetInList(GameManager::Instance().State().Planets().PlayerSubset(&Planet::OwnedBy,Player::enemy()));
}

int Planet::PotentialAttackers() const
{
	try {
		Planet const * closestEnemy = ClosestPlanetInList(GameManager::Instance().State().Planets().PlayerSubset(&Planet::OwnedBy,Player::enemy()));
		return std::max(closestEnemy->ShipsAvailable() - reservedShips, 0);
	} catch (NoPlanetsInListException) {
		return 0;
	}
}

int Planet::MyShipsInTurns( int turns ) const
{
	if (OwnerInTurns(turns) != Player::self()) {
		return 0;
	} else {
		return ShipsInTurns(turns);
	}
}

int Planet::ROI( int turns )
{
	if (OwnerInTurns(turns) == Player::neutral()) {
		return NeutralROI(turns);
	} else if (OwnerInTurns(turns) == Player::enemy())
	{
		return EnemyROI(turns);
	} else 
	{
		return 0;
	}
}

int Planet::ROIFromPlanet(Planet const * p) {
	return ROI(DistanceTo(p));
}

Planet * Planet::ClosestPlanet()
{
	return ClosestPlanetInList(GameManager::Instance().State().Planets());
}

Planet * Planet::ClosestPlanetInList( PlanetList list ) const
{
	list.Remove(this);
	if (list.empty()) {
		throw NoPlanetsInListException();
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
	if (turns == 0)
	{
		if (Owner() == Player::self())
		{
			return ShipsInTurns(turns);
		}
		return ShipsInTurns(turns) + 1;
	}
	if (OwnerInTurns(turns - 1) == Player::self()) {
		return ShipsInTurns(turns);
	}
	return ShipsInTurns(turns) + 1;
}

PlanetState Planet::CurrentState() const
{
	return state;
}

std::vector<PlanetState> const & Planet::FutureStates( unsigned int turns) const
{
	if (futureStates.size() > turns)
	{
		return futureStates;
	}
	StateInTurns(turns);
	return futureStates;
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
		if (ROIFromPlanet(closestNeutral) < 0)
		{
			distanceToNeutral = std::numeric_limits<int>::max();
		}
	} catch (NoPlanetsOwnedByPlayerException e) {
		distanceToNeutral = std::numeric_limits<int>::max();
	}

	return 2*distanceToAlly < distanceToEnemy && distanceToAlly < distanceToNeutral && !IsFront();
}

int Planet::DistanceTo( Planet const * p ) const
{
	double dx = X() - p->X();
	double dy = Y() - p->Y();
	return (int)std::ceil(std::sqrt(dx * dx + dy * dy));
}

Planet const * Planet::ClosestPlanetOwnedBy( Player player ) const
{
	PlanetList playerPlanets = GameManager::Instance().State().Planets().PlayerSubset(&Planet::OwnedBy, player);
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
	try {
		PotentialAttacker()->ReserveShips(PotentialAttackers());
	} catch (NoPlanetsInListException) {
	}
	SendShips(p, ShipsAvailable() - PotentialAttackers());
}

 void Planet::ReinforceOnSafePath( Planet const * p )
{
	try {
		PotentialAttacker()->ReserveShips(PotentialAttackers());
	} catch (NoPlanetsInListException) {
	}
	SendShipsOnSafePath(p, ShipsAvailable() - PotentialAttackers());
}

void Planet::AttemptToTakeover( Planet const * p )
{
	try {
		PotentialAttacker()->ReserveShips(PotentialAttackers());
	} catch (NoPlanetsInListException) {
	}
	SendShips(p, std::min(ShipsAvailable() - PotentialAttackers(), p->ShipsToTakeoverInTurns(DistanceTo(p))));
}

bool Planet::CanTakeover( Planet const* p )
{
	return (ShipsAvailable() - PotentialAttackers()) >= p->ShipsToTakeoverInTurns(DistanceTo(p));
}

bool Planet::IsFront() const
{
	if (OwnerInEndGame() != Player::self()) {
		return false;
	}
	try {
		Planet const * closestEnemy = ClosestPlanetInList(GameManager::Instance().State().Planets().PlayerSubset(&Planet::OwnedBy, Player::enemy()));
		return *(closestEnemy->ClosestPlanetInList(GameManager::Instance().State().Planets().PlayerSubset(&Planet::OwnedBy, Player::self()))) == *this;
	} catch (NoPlanetsInListException) {
		return false;
	}
}

bool Planet::AttackPlanets( PlanetList targets, PlanetList::Prioritiser attackFirst)
{
	bool attackSucceded = false;
	while (targets.size() > 0) {
		Planet * dest = (targets.*attackFirst)(this);
		int straightDistance = DistanceTo(dest);
		int safeDistance = SafePathDistanceTo(dest);
		int destEnemyDistance;
		if (dest->Owner() == Player::neutral())
		{
			try {
				destEnemyDistance = dest->DistanceTo(dest->ClosestPlanetOwnedBy(Player::enemy()));
			} catch (NoPlanetsOwnedByPlayerException) {
				destEnemyDistance = std::numeric_limits<int>::max();
			}
		} else {
			destEnemyDistance = std::numeric_limits<int>::max();
		}
		if (dest != nullptr && (CanTakeover(dest)) && (straightDistance <= destEnemyDistance) && !(dest->ROIFromPlanet(this) < 0)) {
			try {
				if (dest->OptimalAttackTime() <= safeDistance) {
					int shipsToSend = dest->ShipsToTakeoverInTurns(safeDistance);
					try {
						PotentialAttacker()->ReserveShips(PotentialAttackers());
					} catch (NoPlanetsInListException) {
					}
					SendShips(dest, std::min(ShipsAvailable() - PotentialAttackers(), shipsToSend));
				} else if (dest->OptimalAttackTime() <= straightDistance) {
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

Player Planet::OwnerInEndGame() const
{
	return OwnerInTurns(GameManager::Instance().TurnsRemaining());
}

bool Planet::OwnedBy( Player player ) const
{
	return player == Owner();
}

bool Planet::OwnedInEndgameBy( Player player ) const
{
	return player == OwnerInEndGame();
}

void Planet::SendShips( Planet const * const target, int ships )
{
	GameManager::Instance().IssueOrder(this, target, ships);
}

void Planet::SendShipsOnSafePath( Planet const * const target, int ships )
{
	SendShips(SafePathPlanet(target),ships);
}

int Planet::SafePathDistanceTo(Planet const * const target) {
	return DistanceTo(SafePathPlanet(target)) + SafePathPlanet(target)->DistanceTo(target);
}

Planet const * const Planet::SafePathPlanet( Planet const * const target ) const
{
	const float safePathExpansionFactor = std::sqrt(2.0f);
	PlanetList myPlanets = GameManager::Instance().State().Planets().PlayerSubset(&Planet::OwnedBy, Player::self());
	myPlanets.Remove(target);
	myPlanets.Remove(this);
	while (myPlanets.size() > 0)
	{
		
		Planet const * closestAlly = ClosestPlanetInList(myPlanets);
		int closestAllyDist = DistanceTo(closestAlly);
		int closestAllyTargetDist = closestAlly->DistanceTo(target);
		int targetDist = DistanceTo(target);
		int safePathDist = closestAllyDist + closestAllyTargetDist;
		if (closestAllyDist < targetDist && closestAllyTargetDist < targetDist && float(safePathDist)/float(targetDist) < safePathExpansionFactor)
		{
			return closestAlly;
		}
		myPlanets.Remove(closestAlly);
	}
	return target;
}

void Planet::ReserveShips( int numShips )
{
	reservedShips += numShips;
}

void Planet::SetUnsaveable()
{
	canSave = true;
}
