#ifndef NOPLANETSINLISTEXCEPTION_H_
#define NOPLANETSINLISTEXCEPTION_H_
#include <exception>
#include <stdexcept>

class NoPlanetsInListException : public std::runtime_error {
public:
	NoPlanetsInListException();
};

#endif //NOPLANETSINLISTEXCEPTION_H_
