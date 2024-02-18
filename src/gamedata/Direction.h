#pragma once

#include "glm/glm.hpp"

class Direction {
public:
	static inline const glm::vec2 UP = glm::vec2(0, -1);
	static inline const glm::vec2 DOWN = glm::vec2(0, 1);
	static inline const glm::vec2 LEFT = glm::vec2(-1, 0);
	static inline const glm::vec2 RIGHT = glm::vec2(1, 0);
};