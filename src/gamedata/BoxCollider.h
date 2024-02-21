#pragma once

// std library
#include <optional>

// my code
#include "../utils/OptionalVec2.h"
#include "../utils/config/RenderingConfig.h"

// dependencies
#include "glm/glm.hpp"

// top, bottom, left, right
struct Extents {
	std::optional<float> top, bottom, left, right;
};

class BoxCollider {
public:
	// the size of the box collider (uses x and y)
	glm::vec2 size;

	// the extents of the box collider (relative)
	// calculated if and only if size is received
	Extents extents;

	// whether the box collider can collide
	// based on whether box_collider_width and box_collider_height were defined in config
	bool canCollide = false;

	// calculates extents given actor pivot and box collider size
	void calculateExtents(OptionalVec2& pivot, glm::ivec2 actorFrontSize, RenderingConfig& rc);

	// returns whether the collider has extents defined
	// only returns true if all sides of the extents are defined
	bool hasExtents();
};