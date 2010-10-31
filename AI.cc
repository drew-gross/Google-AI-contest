#include "AI.h"
#include "PlanetList.h"
#include "GameState.h"
#include "DontNeedToDefendException.h"
#include "GameManager.h"

void AI::DoTurn() {
	try
	{
		DefensePhase();
		AttackPhase();
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

void AI::AttackPhase()
{
	bool shipsSent = true;

	PlanetList needToAttack = GameManager::Instance().State().Planets().NeedAttacking();
	while (needToAttack.size() > 0 && shipsSent) {
		shipsSent = false;
		Planet* source = GameManager::Instance().State().Planets().OwnedBy(Player::self()).Strongest();
		int shipsToSend = 0;

		PlanetList attackFromHere = needToAttack;
		while (attackFromHere.size() > 0) {
			Planet * dest = attackFromHere.HighestROIFromPlanet(source);
			int sourceDestSeparation = GameState::Distance(source, dest);
			shipsToSend = dest->NumShipsToTakeoverInTurns(sourceDestSeparation);

			if ((source != nullptr) && (dest != nullptr) && (source->NumShipsAvailable() >= shipsToSend)) {
				if (dest->OptimalAttackTime() <= sourceDestSeparation) {
					GameManager::Instance().IssueOrder(source, dest, std::min(shipsToSend, source->NumShipsAvailable()));
					shipsSent = true;
				} else {
					attackFromHere.erase(std::remove(attackFromHere.begin(), attackFromHere.end(), dest), attackFromHere.end());
				}
			} else {
				attackFromHere.erase(std::remove(attackFromHere.begin(), attackFromHere.end(), dest), attackFromHere.end());
				needToAttack.erase(std::remove(needToAttack.begin(), needToAttack.end(), dest), needToAttack.end());
			}
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
			int defenderDefendeeDistance = GameState::Distance(myPlanets[j], needToDefend[i]);
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
			if (defendersBeforeOptimalTime.NumShipsAvailable() > needToDefend[i]->NumShipsInTurns(optimalDefenseTime + 1)) {
				needToDefend[i]->SeekDefenseFrom(defendersBeforeOptimalTime, optimalDefenseTime);
				needToDefend[i]->SeekDefenseFrom(defendersAfterOptimalTime, optimalDefenseTime);
			}
		} catch (DontNeedToDefendException e) {
			//do nothing - continue to next planet that needs defending
		}
	}
}

void AI::SupplyPhase()
{
	PlanetList fronts = GameManager::Instance().State().Planets().Fronts();
	PlanetList myPlanets = GameManager::Instance().State().Planets().OwnedBy(Player::self());
	for (unsigned int i = 0; i < myPlanets.size(); ++i)
	{
		if (myPlanets[i]->ClosestPlanetInList(fronts) != nullptr && myPlanets[i]->ClosestPlanetInList(fronts)->Owner() == Player::self())
		{
			GameManager::Instance().IssueOrder(myPlanets[i], myPlanets[i]->ClosestPlanetInList(fronts), std::min(myPlanets[i]->NumShipsAvailable(), myPlanets[i]->NumShips()));
		}	
	}
}
