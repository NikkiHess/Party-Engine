#include "Actor.h"

void Actor::handleFlipping(bool flipping) {
	// if we don't do flipping, don't bother
	if (!flipping)
		return;

	// if move west, flip actor
	// if move east, unflip actor
	// do NOT handle 0 velocity, keep flip the same in that case
	if (velocity.x < 0) {
		flipped = true;
	}
	else if (velocity.x > 0) {
		flipped = false;
	}
}

void Actor::handleVerticalFacing() {
	// if we have a view_image_back
	if (view.imageBack.image) {
		// velocity y is NEGATIVE if you go up
		if (velocity.y < 0) {
			showBack = true;
		}
		if (velocity.y > 0) {
			showBack = false;
		}
	}
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