#include "Player.h"
#include <stdexcept>

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
	playerNum = -1;
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

int Player::GetPlayerNum() const
{
	return playerNum;
}

std::ostream& operator<<(std::ostream& out, const Player& player)
{
	out << player.GetPlayerNum();
	return out;
}
