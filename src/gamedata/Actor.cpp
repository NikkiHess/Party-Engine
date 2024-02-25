// std library
#include <cmath>

// my code
#include "Actor.h"

void Actor::loadTextures(ResourceManager& resourceManager) {
	// check if the actor's images need to be loaded
	if (!view.image.image && view.image.name != "") {
		view.image.image = resourceManager.loadImageTexture(view.image.name);
	}

	// get the actor's image front/back size
	glm::ivec2 size(0);

	// load in the images' sizes if they haven't been already
	if (view.image.size == glm::ivec2(0)) {
		SDL_QueryTexture(view.image.image, nullptr, nullptr, &size.x, &size.y);
		view.image.size = size;
	}
}

glm::vec2 Actor::getWorldPos(RenderingConfig& renderConfig, glm::vec2 pos) {
	glm::vec2 pivot{
		static_cast<int>(std::round(view.pivot.x.value_or(view.image.size.x * 0.5))),
		static_cast<int>(std::round(view.pivot.y.value_or(view.image.size.y * 0.5)))
	};

	// actor world position in pixel coordinates
	return {
		(pos.x * renderConfig.pixelsPerUnit) - pivot.x,
		(pos.y * renderConfig.pixelsPerUnit) - pivot.y
	};
}

glm::vec2 Actor::getScreenPos(RenderingConfig& renderConfig, glm::vec2 cameraPos) {
	glm::vec2 worldPos = getWorldPos(renderConfig, transform.pos);
	// camera center in pixel coordinates
	glm::vec2 cameraCenter(
		(renderConfig.renderSize.x / 2 - renderConfig.cameraOffset.x * renderConfig.pixelsPerUnit) / renderConfig.zoomFactor,
		(renderConfig.renderSize.y / 2 - renderConfig.cameraOffset.y * renderConfig.pixelsPerUnit) / renderConfig.zoomFactor
	);

	// actor position relative to the camera
	glm::vec2 actorCameraRelativePos = worldPos - glm::vec2(std::round(cameraPos.x), std::round(cameraPos.y));

	// actor screen position, accounting for rendering at screen center
	return cameraCenter + actorCameraRelativePos;
}

bool ActorComparator::operator()(Actor* actor1, Actor* actor2) const {
	return actor1->id < actor2->id;
}

bool RenderOrderComparator::operator()(const Actor* actor1, const Actor* actor2) const {
	// render orders equal? render by y pos
	if (actor1->renderOrder == actor2->renderOrder) {
		// y pos equal? render by id
		if (actor1->transform.pos.y == actor2->transform.pos.y) {
			return actor1->id < actor2->id;
		}

		// y pos not equal? compare
		return actor1->transform.pos.y < actor2->transform.pos.y;
	}
	// render order not equal? compare
	return actor1->renderOrder < actor2->renderOrder;
}