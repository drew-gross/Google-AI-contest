#include "PlanetList.h"

#include "GameState.h"
#include "Utilities.h"

#include <algorithm>
#include "DontNeedToAttackException.h"
#include "GameManager.h"
#include "NoPlanetsInListException.h"
#include "Planet.h"

Planet* PlanetList::Weakest() {
	if (size() == 0) throw NoPlanetsInListException();
	Planet* weakestPlanet = (*this)[0];
	for (unsigned int i = 0; i < size(); ++i) {
		Planet* curPlanet = (*this)[i];
		if (curPlanet->Ships() < weakestPlanet->Ships()) {
			weakestPlanet = curPlanet;
		}
	}
	return weakestPlanet;
}

Planet* PlanetList::WeakestFromPlanet( Planet const * const p )
{
	if (size() == 0) throw NoPlanetsInListException();
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
	if (size() == 0) throw NoPlanetsInListException();
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

PlanetList PlanetList::OwnedBy( Player player ) const
{
	PlanetList p;
	for(unsigned int i = 0; i < size(); ++i) {
		if (operator[](i)->Owner() == player) {
			p.push_back(operator[](i));
		}
	}
	return p;
}

PlanetList PlanetList::NotOwnedBy( Player player ) const
{
	PlanetList p;
	for(unsigned int i = 0; i < size(); ++i) {
		if (operator[](i)->Owner() != player) {
			p.push_back(operator[](i));
		}
	}
	return p;
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

Planet* PlanetList::HighestROIFromPlanet( Planet const * source ) const
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

PlanetList PlanetList::NeedDefending() const
{
	PlanetList p;
	for(unsigned int i = 0; i < size(); ++i) {
		if (operator[](i)->NeedToDefend()) {
			p.push_back(operator[](i));
		}
	}
	return p;
}

PlanetList PlanetList::NeedAttacking() const
{
	PlanetList p;
	for(unsigned int i = 0; i < size(); ++i) {
		if (operator[](i)->NeedToAttack()) {
			p.push_back(operator[](i));
		}
	}
	return p;
}

PlanetList PlanetList::Fronts() const
{
	PlanetList p;
	for(unsigned int i = 0; i < size(); ++i) {
		Planet *curPlanet = operator[](i);
		if (curPlanet->IsFront()) {
			p.push_back(curPlanet);
		}
	}
	return p;
}

void PlanetList::RemoveMatches( Planet* p )
{
	erase(std::remove(begin(), end(), p), end());
}
