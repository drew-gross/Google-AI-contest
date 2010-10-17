#include "Planet.h"

Planet::Planet(int planet_id,
	int owner,
	int num_ships,
	int growth_rate,
	double x,
	double y) {
		planet_id_ = planet_id;
		owner_ = owner;
		num_ships_ = num_ships;
		growth_rate_ = growth_rate;
		x_ = x;
		y_ = y;
}

int Planet::PlanetID() const {
	return planet_id_;
}

int Planet::Owner() const {
	return owner_;
}

int Planet::NumShips() const {
	return num_ships_;
}

int Planet::GrowthRate() const {
	return growth_rate_;
}

double Planet::X() const {
	return x_;
}

double Planet::Y() const {
	return y_;
}

void Planet::Owner(int new_owner) {
	owner_ = new_owner;
}

void Planet::NumShips(int new_num_ships) {
	num_ships_ = new_num_ships;
}

void Planet::AddShips(int amount) {
	num_ships_ += amount;
}

void Planet::RemoveShips(int amount) {
	num_ships_ -= amount;
}
