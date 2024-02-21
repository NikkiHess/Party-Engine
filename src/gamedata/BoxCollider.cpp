// std library
#include <cstdio>

// my code
#include "BoxCollider.h"
#include "../utils/config/RenderingConfig.h"

void BoxCollider::calculateExtents(OptionalVec2& pivot, glm::ivec2 actorFrontSize, RenderingConfig& rc) {
    glm::vec2 center = {
        static_cast<int>(pivot.x.value_or(actorFrontSize.x / 2.0f)),
        static_cast<int>(pivot.y.value_or(actorFrontSize.y / 2.0f))
    };

    // calculate the extents based on pivot and collider size
    // the actor's pivot should act as the center of the box collider
    extents.left = (size.x * rc.pixelsPerUnit / 2.0f) - center.x;
    extents.right =  (size.x * rc.pixelsPerUnit / 2.0f) + center.x;
    extents.top = (size.y * rc.pixelsPerUnit / 2.0f) - center.y;
    extents.bottom = (size.y * rc.pixelsPerUnit / 2.0f) + center.y;
}

bool BoxCollider::hasExtents() {
	return extents.bottom.has_value() && extents.top.has_value() && extents.left.has_value() && extents.right.has_value();
}