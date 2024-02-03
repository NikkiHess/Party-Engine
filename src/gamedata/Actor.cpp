// std stuff
#include <iostream>

// my code
#include "Actor.h"

void Actor::printContactDialogue() const {
	std::cout << contactDialogue << "\n";
}

void Actor::printNearbyDialogue() const {
	std::cout << nearbyDialogue << "\n";
}