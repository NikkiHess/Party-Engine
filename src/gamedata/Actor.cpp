#include "Actor.h"

void Actor::handleFlipping(bool flipping) {
	// if we don't do flipping, don't bother
	if (!flipping)
		return;

	// if move west, flip actor
	// if move east, unflip actor
	// do NOT handle 0 velocity, keep flip the same in that case
	if (velocity.x < 0) {
		transform.flipped = true;
	}
	else if (velocity.x > 0) {
		transform.flipped = false;
	}
}

void Actor::handleVerticalFacing() {
	// if we have a view_image_back
	if (view.imageBack.image) {
		// velocity y is NEGATIVE if you go up
		if (velocity.y < 0) {
			transform.showBack = true;
		}
		if (velocity.y > 0) {
			transform.showBack = false;
		}
	}
}

void Actor::loadTextures(ResourceManager& resourceManager) {
	// check if the actor's images need to be loaded
	if (!view.imageFront.image && view.imageFront.name != "") {
		view.imageFront.image = resourceManager.loadImageTexture(view.imageFront.name);
	}
	if (!view.imageBack.image && view.imageBack.name != "") {
		view.imageBack.image = resourceManager.loadImageTexture(view.imageBack.name);
	}

	// get the actor's image front/back size
	glm::ivec2 size(0);

	// load in the images' sizes if they haven't been already
	if (view.imageFront.size == glm::ivec2(0)) {
		SDL_QueryTexture(view.imageFront.image, nullptr, nullptr, &size.x, &size.y);
		view.imageFront.size = size;
	}
	if (view.imageBack.size == glm::ivec2(0)) {
		SDL_QueryTexture(view.imageBack.image, nullptr, nullptr, &size.x, &size.y);
		view.imageBack.size = size;
	}
}

glm::vec2 Actor::getWorldPos(RenderingConfig& renderConfig, glm::vec2 pos) {
	glm::vec2 pivot{
		static_cast<int>(std::round(view.pivot.x.value_or(view.imageFront.size.x * 0.5))),
		static_cast<int>(std::round(view.pivot.y.value_or(view.imageFront.size.y * 0.5)))
	};

	// actor world position in pixel coordinates
	return {
		(pos.x * renderConfig.pixelsPerUnit) - pivot.x,
		(pos.y * renderConfig.pixelsPerUnit) - pivot.y
	};
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