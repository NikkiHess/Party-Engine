// std library
#include <unordered_map>
#include <string>

// my code
#include "Input.h"

// dependencies
#include "SDL2/SDL.h"
#include "Helper.h"
#include "glm/glm.hpp"

void Input::init() {
	for (int code = SDL_SCANCODE_UNKNOWN; code < SDL_NUM_SCANCODES; ++code) {
		keyboardStates[static_cast<SDL_Scancode>(code)] = InputState::UP;
	}
}

void Input::processEvent(const SDL_Event& sdlEvent) {
	// handle newly up keys
	if (sdlEvent.type == SDL_KEYUP) {
		SDL_Scancode code = sdlEvent.key.keysym.scancode;
		keyboardStates[code] = InputState::NEWLY_UP;
		newlyUpKeycodes.emplace(code);
	}
	// handle newly down keys
	else if (sdlEvent.type == SDL_KEYDOWN) {
		SDL_Scancode code = sdlEvent.key.keysym.scancode;
		keyboardStates[code] = InputState::NEWLY_DOWN;
		newlyDownKeycodes.emplace(code);
	}
	else if (sdlEvent.type == SDL_MOUSEMOTION) {
		mousePos = { sdlEvent.motion.x, sdlEvent.motion.y };
	}
	else if (sdlEvent.type == SDL_MOUSEBUTTONUP) {
		int button = sdlEvent.button.button;
		buttonStates[button] = InputState::NEWLY_UP;
		newlyUpButtons.emplace(button);
	}
	else if (sdlEvent.type == SDL_MOUSEBUTTONDOWN) {
		int button = sdlEvent.button.button;
		buttonStates[button] = InputState::NEWLY_DOWN;
		newlyDownButtons.emplace(button);
	}
	else if (sdlEvent.type == SDL_MOUSEWHEEL) {
		mouseScrollDelta = sdlEvent.wheel.preciseY;
	}
}

void Input::lateUpdate() {
	// progress NEWLY_UP keycodes to UP
	for (const SDL_Scancode& code : newlyUpKeycodes) {
		keyboardStates[code] = InputState::UP;
	}
	newlyUpKeycodes.clear();

	// progress NEWLY_DOWN keycodes to DOWN
	for (const SDL_Scancode& code : newlyDownKeycodes) {
		keyboardStates[code] = InputState::DOWN;
	}
	newlyDownKeycodes.clear();

	// progress NEWLY_UP buttons to UP
	for (int button : newlyUpButtons) {
		buttonStates[button] = InputState::UP;
	}
	newlyUpButtons.clear();

	// progress NEWLY_DOWN buttons to DOWN
	for (int button : newlyDownButtons) {
		buttonStates[button] = InputState::DOWN;
	}
	newlyDownButtons.clear();

	mouseScrollDelta = 0;
}

bool Input::getKey(const std::string& code) {
	auto it = keycodeToScancode.find(code);
	if (it != keycodeToScancode.end()) {
		SDL_Scancode scancode = it->second;
		return keyboardStates[scancode] == InputState::DOWN || keyboardStates[scancode] == InputState::NEWLY_DOWN;
	}
	return false;
}

bool Input::getKeyDown(const std::string& code) {
	auto it = keycodeToScancode.find(code);
	if (it != keycodeToScancode.end()) {
		SDL_Scancode scancode = it->second;
		return keyboardStates[scancode] == InputState::NEWLY_DOWN;
	}
	return false;
}

bool Input::getKeyUp(const std::string& code) {
	auto it = keycodeToScancode.find(code);
	if (it != keycodeToScancode.end()) {
		SDL_Scancode scancode = it->second;
		return keyboardStates[scancode] == InputState::NEWLY_UP;
	}
	return false;
}

glm::vec2 Input::getMousePosition() {
	return mousePos;
}

// check whether a mouse button is down
// 1 = left, 2 = middle, 3 = right
bool Input::getMouseButton(int button) {
	return buttonStates.find(button) != buttonStates.end() && 
		(buttonStates[button] == InputState::DOWN || buttonStates[button] == InputState::NEWLY_DOWN);
}

// check whether a mouse button is newly down
// 1 = left, 2 = middle, 3 = right
bool Input::getMouseButtonDown(int button) {
	return buttonStates.find(button) != buttonStates.end() && buttonStates[button] == InputState::NEWLY_DOWN;
}

// check whether a mouse button is newly up
// 1 = left, 2 = middle, 3 = right
bool Input::getMouseButtonUp(int button) {
	return buttonStates.find(button) != buttonStates.end() && buttonStates[button] == InputState::NEWLY_UP;
}

// check how far the mouse has scrolled this frame
float Input::getMouseScrollDelta() {
	return mouseScrollDelta;
}