#include "PlanetList.h"

#include "PlanetWars.h"
#include "Utilities.h"

#include <algorithm>
#include "DontNeedToAttackException.h"

Planet* PlanetList::Weakest() {
	if (size() == 0) return nullptr;
	Planet* weakestPlanet = (*this)[0];
	for (unsigned int i = 0; i < size(); ++i) {
		Planet* curPlanet = (*this)[i];
		if (curPlanet->NumShips() < weakestPlanet->NumShips()) {
			weakestPlanet = curPlanet;
		}
	}
	return weakestPlanet;
}

Planet* PlanetList::WeakestFromPlanet(Planet const& p) {
	if (size() == 0) return nullptr;
	Planet* weakestPlanet = (*this)[0];
	for (unsigned int i = 0; i < size(); ++i) {
		Planet* curPlanet = (*this)[i];
		if (curPlanet->NumShipsInTurns(PlanetWars::Distance(p, *curPlanet)) < weakestPlanet->NumShipsInTurns(PlanetWars::Distance(p, *curPlanet))) {
			weakestPlanet = curPlanet;
		}
	}
	return weakestPlanet;
}

Planet* PlanetList::Strongest() {
	if (size() == 0) return nullptr;
	Planet* strongestPlanet = (*this)[0];
	for (unsigned int i = 0; i < size(); ++i) {
		Planet* curPlanet = (*this)[i];
		if (curPlanet->NumShips() > strongestPlanet->NumShips()) {
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


class isNotOwnedBy {
public:
	isNotOwnedBy(Player newPlayer) : player(newPlayer) {
	}
	bool operator()(Planet * planet) {
		return planet->Owner() != player;
	}
private:
	Player player;
};

PlanetList& PlanetList::OwnedBy(Player player) {
	this->erase(remove_if(this->begin(), this->end(), isNotOwnedBy(player)), this->end());
	return *this;
}

class isOwnedBy {
public:
	isOwnedBy(Player newPlayer) : player(newPlayer) {
	}
	bool operator()(Planet * planet) {
		return planet->Owner() == player;
	}
private:
	Player player;
};

PlanetList& PlanetList::NotOwnedBy(Player player) {
	this->erase(remove_if(this->begin(), this->end(), isOwnedBy(player)), this->end());
	return *this;
}

int PlanetList::NumShipsAvailable()
{
	int shipsAvailable = 0;
	for (unsigned int i = 0; i < size(); ++i)
	{
		shipsAvailable += operator[](i)->NumShipsAvailable();
	}
	return shipsAvailable;
}

Planet* PlanetList::HighestROIFromPlanet( Planet const * source ) const
{
	CreateLogger(debug);
	int ROI = std::numeric_limits<int>::max();
	Planet * highestROIPlanet = operator[](0);
	for (unsigned int i = 0; i < size(); ++i)
	{
		debug.Log("Beginning of HROI loop");
		debug.LogVar(i);
		Planet * curPlanet = operator[](i);
		try
		{
			debug.Log("calling ROI");
			int curPlanetROI = curPlanet->ReturnOnInvestment(PlanetWars::Distance(*source, *curPlanet));
			if (curPlanetROI > ROI) {
				ROI = curPlanetROI;
				highestROIPlanet = curPlanet;
			}
		}
		catch (DontNeedToAttackException e) 
		{
			debug.Log("exception caught");
			//do nothing, for loop will go to next planet anyway.
		}
	}
	return highestROIPlanet;
}
