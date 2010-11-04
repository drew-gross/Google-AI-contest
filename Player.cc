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

Player Player::Opponent()
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

bool Player::operator==( Player const & rhs )
{
	return playerNum == rhs.playerNum;
}

bool Player::operator!=( Player const & rhs )
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

bool Player::IsAlive() const {
	return (Ships() > 0);
}

int Player::Ships() const {
	int num_ships = 0;
	for (unsigned int i = 0; i < GameManager::Instance().State().Planets().size(); ++i) {
		if (GameManager::Instance().State().Planets()[i]->Owner() == playerNum) {
			num_ships += GameManager::Instance().State().Planets()[i]->Ships();
		}
	}
	for (unsigned int i = 0; i < GameManager::Instance().State().Fleets().size(); ++i) {
		if (GameManager::Instance().State().Fleets()[i]->Owner() == playerNum) {
			num_ships += GameManager::Instance().State().Fleets()[i]->Ships();
		}
	}
	return num_ships;
}
