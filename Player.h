#ifndef Player_h__
#define Player_h__

#include <iostream>
#include <fstream>
#include <ostream>

class Player {
public:
	Player();
	Player (int newPlayerNum);

	int PlayerNum() const;

	Player Opponent();

	bool operator==(Player const & rhs);
	bool operator!=(Player const & rhs);

	static Player neutral();
	static Player self();
	static Player enemy();

	// Returns the number of ships that the given player has, either located
	// on planets or in flight.
	int Ships() const;

private:
	int playerNum;
};

std::ostream& operator<<(std::ostream& out, const Player& player);

#endif // Player_h__
