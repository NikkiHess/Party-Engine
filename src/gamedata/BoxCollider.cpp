// std library
#include <iostream>

// my code
#include "BoxCollider.h"

void BoxCollider::calculateExtents(OptionalVec2& pivot, glm::ivec2 actorFrontSize) {
	std::cout << pivot.x.value_or(actorFrontSize.x * 0.5f) << ", " << pivot.y.value_or(actorFrontSize.y * 0.5f) << "\n";
}

bool BoxCollider::hasExtents() {
	return extents.bottom.has_value() && extents.top.has_value() && extents.left.has_value() && extents.right.has_value();
}