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

void Camera::setZoom(const float zoom) {
	renderConfig->zoomFactor = zoom;
}

float Camera::getZoom() {
	return renderConfig->zoomFactor;
}