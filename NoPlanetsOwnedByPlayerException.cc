#include "NoPlanetsOwnedByPlayerException.h"

NoPlanetsOwnedByPlayerException::NoPlanetsOwnedByPlayerException(Player newPlayer) :  runtime_error("NoPlanetsOwnedByPlayerException"), player(newPlayer) {
}

NoPlanetsOwnedByPlayerException::~NoPlanetsOwnedByPlayerException() throw() {
}

Player NoPlanetsOwnedByPlayerException::GetPlayer() {
	return player;
}
