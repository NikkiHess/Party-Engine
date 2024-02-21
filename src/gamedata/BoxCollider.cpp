// std library
#include <cstdio>

// my code
#include "BoxCollider.h"

void BoxCollider::calculateExtents(OptionalVec2& pivot, glm::ivec2 actorFrontSize) {
	// the center, determined by the pivot or the center of the actor's front
	glm::vec2 center(pivot.x.value_or(actorFrontSize.x * 0.5f), pivot.y.value_or(actorFrontSize.y * 0.5f));

	extents.top = center.y - (size.y / 2);
	extents.bottom = center.y + (size.y / 2);
	extents.left = center.x - (size.x / 2);
	extents.right = center.x + (size.x / 2);
}

bool BoxCollider::hasExtents() {
	return extents.bottom.has_value() && extents.top.has_value() && extents.left.has_value() && extents.right.has_value();
}