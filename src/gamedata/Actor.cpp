// std stuff
#include <iostream>

// my code
#include "Actor.h"

void Actor::printContactDialogue() const {
	if(contactDialogue != "")
		std::cout << contactDialogue << "\n";
}

void Actor::printNearbyDialogue() const {
	if(nearbyDialogue != "")
		std::cout << nearbyDialogue << "\n";
}