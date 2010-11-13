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
	while (GameManager::Instance().State().Planets().NeedAttacking().size() > 0) {
		Planet* source;
		try {
			source = GameManager::Instance().State().Planets().OwnedBy(Player::self()).Strongest();
		} catch (NoPlanetsInListException e) {
			return;
		}
		if (!(source->AttackPlanets(GameManager::Instance().State().Planets().NeedAttacking()))) {
			return;
		}
	}
}

void AI::CautiousAttackPhase()
{
	while (GameManager::Instance().State().Planets().NeedAttackingCautiously().size() > 0) {
		Planet* source;
		try {
			source = GameManager::Instance().State().Planets().OwnedBy(Player::self()).Strongest();
		} catch (NoPlanetsInListException e) {
			return;
		}
		if (!(source->AttackPlanets(GameManager::Instance().State().Planets().NeedAttackingCautiously()))) {
			return;
		}
	}
}

void AI::DefensePhase() {
	PlanetList needToDefend = GameManager::Instance().State().Planets().NeedDefending();
	for (unsigned int i = 0; i < needToDefend.size(); ++i) {
		int optimalDefenseTime = needToDefend[i]->OptimalDefenseTime();
		PlanetList defendersAtOptimalTime;
		PlanetList defendersAfterOptimalTime;
		PlanetList defendersBeforeOptimalTime;

		PlanetList myPlanets = GameManager::Instance().State().Planets().OwnedBy(Player::self());
		for (unsigned int j = 0; j < myPlanets.size(); ++j) {
			int defenderDefendeeDistance = myPlanets[j]->DistanceTo(needToDefend[i]);
			if (defenderDefendeeDistance == optimalDefenseTime) {
				defendersAtOptimalTime.push_back(myPlanets[j]);
			} else if (defenderDefendeeDistance < optimalDefenseTime) {
				defendersBeforeOptimalTime.push_back(myPlanets[j]);
			} else if (defenderDefendeeDistance > optimalDefenseTime) {
				defendersAfterOptimalTime.push_back(myPlanets[j]);
			}
		}
		try {
			needToDefend[i]->SeekDefenseFrom(defendersAtOptimalTime, optimalDefenseTime);
			if (defendersBeforeOptimalTime.ShipsAvailable() > needToDefend[i]->ShipsInTurns(optimalDefenseTime)) {
				needToDefend[i]->SeekDefenseFrom(defendersBeforeOptimalTime, optimalDefenseTime);
				needToDefend[i]->SeekDefenseFrom(defendersAfterOptimalTime, optimalDefenseTime);
			}
		} catch (DontNeedToDefendException e) {
			continue;
		}
	}
}

void AI::SupplyPhase()
{
	PlanetList myPlanets = GameManager::Instance().State().Planets().OwnedBy(Player::self());
	for (unsigned int i = 0; i < myPlanets.size(); ++i)
	{
		if (myPlanets[i]->IsSupplier())
		{
			try {
				Planet * source = myPlanets[i];
				Planet const * dest = source->ClosestPlanetInList(GameManager::Instance().State().Planets().OwnedBy(Player::self()).Fronts());
				source->Reinforce(dest);
			} catch (NoPlanetsOwnedByPlayerException e) {
				break;
			}
		}	
	}
}
