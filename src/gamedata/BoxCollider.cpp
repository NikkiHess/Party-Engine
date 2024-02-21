// std library
#include <iostream>

// my code
#include "BoxCollider.h"

void BoxCollider::calculateExtents(OptionalVec2& pivot) {
	std::cout << pivot.x.value() << ", " << pivot.y.value() << "\n";
}

bool BoxCollider::hasExtents() {
	return extents.bottom != 0 || extents.top != 0 || extents.left != 0 || extents.right != 0;
}