#include "PlanetList.h"

#include "GameState.h"
#include "Utilities.h"

#include <algorithm>
#include "DontNeedToAttackException.h"
#include "GameManager.h"
#include "NoPlanetsInListException.h"
#include "Planet.h"

Planet* PlanetList::Weakest() {
	if (size() == 0) {
		throw NoPlanetsInListException();
	}
	Planet* weakestPlanet = (*this)[0];
	for (unsigned int i = 0; i < size(); ++i) {
		Planet* curPlanet = (*this)[i];
		if (curPlanet->Ships() < weakestPlanet->Ships()) {
			weakestPlanet = curPlanet;
		}
	}
	return weakestPlanet;
}

Planet* PlanetList::WeakestFromPlanet( Planet const * const p ) const
{
	if (size() == 0) {
		throw NoPlanetsInListException();
	}
	Planet* weakestPlanet = (*this)[0];
	for (unsigned int i = 0; i < size(); ++i) {
		Planet* curPlanet = (*this)[i];
		if (curPlanet->ShipsInTurns(p->DistanceTo(curPlanet)) < weakestPlanet->ShipsInTurns(p->DistanceTo(curPlanet))) {
			weakestPlanet = curPlanet;
		}
	}
	return weakestPlanet;
}

Planet* PlanetList::Strongest() {
	if (size() == 0) {
		throw NoPlanetsInListException();
	}
	Planet* strongestPlanet = (*this)[0];
	for (unsigned int i = 0; i < size(); ++i) {
		Planet* curPlanet = (*this)[i];
		if (curPlanet->Ships() > strongestPlanet->Ships()) {
			strongestPlanet = curPlanet;
		}
	}
	return strongestPlanet;
}

void PlanetList::DeleteAll() {
	for (unsigned int i = 0; i < size(); i++) {
		delete (*this)[i];
	}
	this->resize(0);
}

PlanetList PlanetList::OwnedInTurnsBy( Player player, int turns ) const
{
	PlanetList p;
	for(unsigned int i = 0; i < size(); ++i) {
		if (operator[](i)->OwnerInTurns(turns) == player) {
			p.push_back(operator[](i));
		}
	}
	return p;
}

PlanetList PlanetList::PlayerSubset( PlanetList::PlayerSet matcher, Player player ) const
{
	PlanetList p;
	for(unsigned int i = 0; i < size(); ++i) {
		if ((operator[](i)->*matcher)(player)) {
			p.push_back(operator[](i));
		}
	}
	return p;
}

int PlanetList::Ships() const
{
	int shipsAvailable = 0;
	for (unsigned int i = 0; i < size(); ++i)
	{
		shipsAvailable += operator[](i)->Ships();
	}
	return shipsAvailable;
}

int PlanetList::ShipsAvailable()
{
	int shipsAvailable = 0;
	for (unsigned int i = 0; i < size(); ++i)
	{
		shipsAvailable += operator[](i)->ShipsAvailable();
	}
	return shipsAvailable;
}

Planet* PlanetList::HighestROIFromPlanet( Planet const * const source ) const
{
	int ROI = std::numeric_limits<int>::min();
	Planet * highestROIPlanet = operator[](0);
	for (unsigned int i = 0; i < size(); ++i)
	{
		Planet * curPlanet = operator[](i);
		try
		{
			int curPlanetROI = curPlanet->ReturnOnInvestment(source->DistanceTo(curPlanet));
			if (curPlanetROI > ROI) {
				ROI = curPlanetROI;
				highestROIPlanet = curPlanet;
			}
		}
		catch (DontNeedToAttackException e) 
		{
			break;
		}
	}
	return highestROIPlanet;
}

Planet* PlanetList::HighestGrowthEnemy( Planet const * const source ) const
{
	int highestGrowth = std::numeric_limits<int>::min();
	Planet * highestGrowthEnemy = operator[](0);
	for (unsigned int i = 0; i < size(); ++i)
	{
		Planet * curPlanet = operator[](i);
		if (curPlanet->Owner() != Player::enemy()) {
			continue;
		}
		int curPlanetGrowth = curPlanet->Growth();
		if (curPlanetGrowth > highestGrowth) {
			highestGrowth = curPlanetGrowth;
			highestGrowthEnemy = curPlanet;
		}
	}
	return highestGrowthEnemy;
}

PlanetList PlanetList::Subset( PlanetList::Set matcher ) const
{
	PlanetList p;
	for(unsigned int i = 0; i < size(); ++i) {
		if ((operator[](i)->*matcher)()) {
			p.push_back(operator[](i));
		}
	}
	return p;

}

void PlanetList::Remove( Planet const * const p )
{
	erase(std::remove(begin(), end(), p), end());
}

bool LowestGrowth(Planet const * const first, Planet const * const second) {
	if (first->Growth() < second->Growth()) {
		return true;
	} else {
		return false;
	}
}

void PlanetList::SortByHighestGrowth()
{
	std::sort(begin(), end(), &LowestGrowth);
}
