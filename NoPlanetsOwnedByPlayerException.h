#ifndef NO_PLANETS_OWNED_BY_PLAYER
#define NO_PLANETS_OWNED_BY_PLAYER

#include <stdexcept>

#include "Planet.h"

class NoPlanetsOwnedByPlayerException : public std::runtime_error {
public:
	NoPlanetsOwnedByPlayerException(Player newPlayer);
	~NoPlanetsOwnedByPlayerException() throw();

	Player GetPlayer();
private:
	Player player;
};

#endif //NO_PLANETS_OWNED_BY_PLAYER
