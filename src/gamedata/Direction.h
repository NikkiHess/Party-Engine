#pragma once

#include "glm/glm.hpp"

class Direction {
public:
	static inline const glm::vec2 UP = glm::ivec2(0, -1);
	static inline const glm::vec2 DOWN = glm::ivec2(0, 1);
	static inline const glm::vec2 LEFT = glm::ivec2(-1, 0);
	static inline const glm::vec2 RIGHT = glm::ivec2(1, 0);
};