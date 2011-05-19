#include "AI.h"
#include "PlanetList.h"
#include "GameState.h"
#include "DontNeedToDefendException.h"
#include "GameManager.h"
#include "DontNeedToAttackException.h"
#include "NoPlanetsOwnedByPlayerException.h"
#include "NoPlanetsInListException.h"

void AI::DoTurn() {
	try
	{
		DefensePhase();
		if (Player::self().IsWinning()){ 
			CautiousAttackPhase();
		} else {
			AgressiveAttackPhase();
		}
		SupplyPhase();
	}
 	catch (std::exception e)
 	{
 		CreateStaticLogger(uncaughtExceptions);
 		uncaughtExceptions.Log(e.what());
 	}
}

void AI::FinishTurn() {
	GameManager::Instance().State().DeleteData();
	std::cout << "go" << std::endl;
	std::cout.flush();
}

void AI::AgressiveAttackPhase()
{
	while (GameManager::Instance().State().Planets().Subset(&Planet::NeedToAttack).size() > 0) {
		Planet* source;
		try {
			source = GameManager::Instance().State().Planets().PlayerSubset(&Planet::OwnedBy,Player::self()).Strongest();
		} catch (NoPlanetsInListException) {
			return;
		}
		if (!(source->AttackPlanets(GameManager::Instance().State().Planets().Subset(&Planet::NeedToAttack), &PlanetList::HighestROIFromPlanet))) {
			return;
		}
	}
}

void AI::CautiousAttackPhase()
{
	while (GameManager::Instance().State().Planets().Subset(&Planet::NeedToAttackCautiously).size() > 0) {
		Planet* source;
		try {
			source = GameManager::Instance().State().Planets().PlayerSubset(&Planet::OwnedBy, Player::self()).Strongest();
		} catch (NoPlanetsInListException) {
			return;
		}
		if (!(source->AttackPlanets(GameManager::Instance().State().Planets().Subset(&Planet::NeedToAttackCautiously), &PlanetList::HighestGrowthEnemy))) {
			return;
		}
	}
}

void AI::DefensePhase() {
	PlanetList needToDefend = GameManager::Instance().State().Planets().Subset(&Planet::NeedToDefend);
	needToDefend.SortByHighestGrowth();
	for (unsigned int i = 0; i < needToDefend.size(); ++i) {
		int optimalDefenseTime = needToDefend[i]->OptimalDefenseTime();
		PlanetList defendersAtOptimalTime;
		PlanetList defendersAfterOptimalTime;
		PlanetList defendersBeforeOptimalTime;

		PlanetList myPlanets = GameManager::Instance().State().Planets().PlayerSubset(&Planet::OwnedBy,Player::self());
		for (unsigned int j = 0; j < myPlanets.size(); ++j) {
			int distance = myPlanets[j]->DistanceTo(needToDefend[i]);
			if (distance == optimalDefenseTime) {
				defendersAtOptimalTime.push_back(myPlanets[j]);
			} else if (distance < optimalDefenseTime) {
				defendersBeforeOptimalTime.push_back(myPlanets[j]);
			} else if (distance > optimalDefenseTime) {
				defendersAfterOptimalTime.push_back(myPlanets[j]);
			}
		}
		if (defendersAfterOptimalTime.ShipsAvailable() + defendersAtOptimalTime.ShipsAvailable() + defendersBeforeOptimalTime.ShipsAvailable() < needToDefend[i]->ShipsInTurns(optimalDefenseTime) )
		{
			needToDefend[i]->SetUnsaveable();
		} else {
			try {
				defendersAtOptimalTime.SortByClosestTo(needToDefend[i]);
				defendersAfterOptimalTime.SortByClosestTo(needToDefend[i]);
				defendersBeforeOptimalTime.SortByClosestTo(needToDefend[i]);

				needToDefend[i]->SeekDefenseFrom(defendersAtOptimalTime, optimalDefenseTime);
				if (defendersBeforeOptimalTime.ShipsAvailable() < needToDefend[i]->ShipsInTurns(optimalDefenseTime)) {
					needToDefend[i]->SeekDefenseFrom(defendersBeforeOptimalTime, optimalDefenseTime);
					needToDefend[i]->SeekDefenseFrom(defendersAfterOptimalTime, optimalDefenseTime);
				} else {
					needToDefend[i]->ReserveDefenseFrom(defendersBeforeOptimalTime, optimalDefenseTime);
				}
			} catch (DontNeedToDefendException e) {
				continue;
			}
		}
	}
}

void AI::SupplyPhase()
{
	PlanetList myPlanets = GameManager::Instance().State().Planets().PlayerSubset(&Planet::OwnedBy, Player::self());
	PlanetList fronts = GameManager::Instance().State().Planets().Subset(&Planet::IsFront);
	for (unsigned int i = 0; i < myPlanets.size(); ++i)
	{
		try {
			Planet * source = myPlanets[i];
			Planet const * closestFront = source->ClosestPlanetInList(fronts);
			if (source->IsSupplier())
			{
				source->ReinforceOnSafePath(closestFront);
			} else {
				Planet const * closestNeutral = source->ClosestPlanetInList(GameManager::Instance().State().Planets().PlayerSubset(&Planet::OwnedBy, Player::neutral()));
				Planet const * closestEnemy = source->ClosestPlanetInList(GameManager::Instance().State().Planets().PlayerSubset(&Planet::OwnedBy, Player::enemy()));
				Planet const * dest = closestNeutral->ClosestPlanetInList(GameManager::Instance().State().Planets().PlayerSubset(&Planet::OwnedBy, Player::self()));
				if (dest->DistanceTo(closestFront) < source->DistanceTo(closestFront) && dest->DistanceTo(source) < source->DistanceTo(closestEnemy))
				{
					source->ReinforceOnSafePath(dest);
				}
			}
		} catch (NoPlanetsOwnedByPlayerException e) {
			break;
		} catch (NoPlanetsInListException e) {
			break;
		}
	}
}
