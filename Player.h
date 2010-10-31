#ifndef Player_h__
#define Player_h__

#include <iostream>
#include <fstream>
#include <ostream>

class Player {
public:
	Player();
	Player (int newPlayerNum);

	int GetPlayerNum() const;

	Player Opponent();

	bool operator==(Player const & rhs);
	bool operator!=(Player const & rhs);

	static Player neutral();
	static Player self();
	static Player enemy();

	// Returns true if the named player owns at least one planet or fleet.
	// Otherwise, the player is deemed to be dead and false is returned.
	bool IsAlive() const;

	// Returns the number of ships that the given player has, either located
	// on planets or in flight.
	int NumShips() const;

private:
	int playerNum;
};

std::ostream& operator<<(std::ostream& out, const Player& player);

#endif // Player_h__
