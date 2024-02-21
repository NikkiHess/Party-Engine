#pragma once

// my code
#include "../utils/OptionalVec2.h"

// dependencies
#include "glm/glm.hpp"

struct Extents {
	float top = 0, bottom = 0, left = 0, right = 0;
};

class BoxCollider {
public:
	// the size of the box collider (uses x and y)
	glm::vec2 size;

	// the extents of the box collider (relative)
	// calculated if and only if size is received
	Extents extents;

	// calculates extents given actor pivot and box collider size
	void calculateExtents(OptionalVec2& pivot);

	// returns whether the collider has extents defined
	bool hasExtents();
};