// std library
#include <cstdio>
#include <cmath>

// my code
#include "BoxCollider.h"
#include "../utils/config/RenderingConfig.h"

void BoxCollider::calculateExtents(OptionalVec2& pivot, glm::ivec2 actorFrontSize, RenderingConfig& rc) {
    glm::vec2 center = {
        static_cast<int>(std::round(pivot.x.value_or(actorFrontSize.x * 0.5f))),
        static_cast<int>(std::round(pivot.y.value_or(actorFrontSize.y * 0.5f)))
    };

    // calculate the extents based on pivot and collider size
    // the actor's pivot should act as the center of the box collider
    extents.top = center.y - (size.y * rc.pixelsPerUnit / 2.0f);
    extents.bottom = center.y + (size.y * rc.pixelsPerUnit / 2.0f);
    extents.left = center.x - (size.x * rc.pixelsPerUnit / 2.0f);
    extents.right = center.x + (size.x * rc.pixelsPerUnit / 2.0f);
}

bool BoxCollider::hasExtents() {
	return extents.bottom.has_value() && extents.top.has_value() && extents.left.has_value() && extents.right.has_value();
}

Extents BoxCollider::getScreenExtents(RenderingConfig& rc, glm::vec2 actorScreenPos) {
    float halfHeight = size.y * rc.pixelsPerUnit / 2.0f;
    float halfWidth = size.x * rc.pixelsPerUnit / 2.0f;
    if (hasExtents()) {
        Extents screenExtents = {
            actorScreenPos.y - halfHeight,
            actorScreenPos.y + halfHeight,
            actorScreenPos.x - halfWidth,
            actorScreenPos.x + halfWidth
        };
        return screenExtents;
    }
    return Extents();
}