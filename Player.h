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

	Player Opponent() const;

	bool operator==(Player const & rhs) const;
	bool operator!=(Player const & rhs) const;

	static Player neutral();
	static Player self();
	static Player enemy();

	int Ships() const;

	int Growth() const;
	int GrowthInTurns(int turns) const;

	bool IsWinning() const;
private:
	int playerNum;
};

std::ostream& operator<<(std::ostream& out, const Player& player);

#endif // Player_h__
