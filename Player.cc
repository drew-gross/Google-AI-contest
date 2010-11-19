#include "Player.h"
#include <stdexcept>
#include "GameManager.h"

Player Player::neutral()
{
	return Player(0);
}

Player Player::self()
{
	return Player(1);
}

Player Player::enemy()
{
	return Player(2);
}

Player::Player( int newPlayerNum ) : playerNum(newPlayerNum)
{

}

Player::Player()
{
}

Player Player::Opponent() const
{
	if (*this == self())
	{
		return enemy();
	} 
	else if (*this == enemy())
	{
		return self();
	}
	throw std::logic_error("Attempted to find the opponent of neutral!");
}

bool Player::operator==( Player const & rhs ) const
{
	return playerNum == rhs.playerNum;
}

bool Player::operator!=( Player const & rhs ) const
{
	return playerNum != rhs.playerNum;
}

int Player::PlayerNum() const
{
	return playerNum;
}

std::ostream& operator<<(std::ostream& out, const Player& player)
{
	out << player.PlayerNum();
	return out;
}

int Player::Ships() const {
	int num_ships = 0;
	num_ships += GameManager::Instance().State().Planets().PlayerSubset(&Planet::OwnedBy, *this).Ships();
	num_ships += GameManager::Instance().State().Forces().ShipsFromPlayer(*this);
	return num_ships;
}

int Player::Growth() const
{
	int growth = 0;
	PlanetList myPlanets = GameManager::Instance().State().Planets().PlayerSubset(&Planet::OwnedBy, *this);
	for (unsigned int i = 0; i < myPlanets.size(); ++i) {
		growth += myPlanets[i]->Growth();
	}
	return growth;
}

int Player::GrowthInTurns( int turns ) const
{
	int growth = 0;
	PlanetList myPlanets = GameManager::Instance().State().Planets().OwnedInTurnsBy(*this, turns);
	for (unsigned int i = 0; i < myPlanets.size(); ++i) {
		growth += myPlanets[i]->Growth();
	}
	return growth;
}

bool Player::IsWinning() const
{
	return GrowthInTurns(GameManager::Instance().State().MaxDistance()) > Opponent().GrowthInTurns(GameManager::Instance().State().MaxDistance()) && Ships() > Opponent().Ships();
}
