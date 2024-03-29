// my code
#include "Camera.h"

void Camera::setPosition(const float x, const float y) {
	pos.x = x;
	pos.y = y;
}

float Camera::getPositionX() {
	return pos.x;
}

float Camera::getPositionY() {
	return pos.y;
}