// std library
#include <unordered_map>
#include <string>

// my code
#include "Input.h"

// dependencies
#include "SDL2/SDL.h"

void Input::init() {
	for (int code = SDL_SCANCODE_UNKNOWN; code < SDL_NUM_SCANCODES; ++code) {
		keyboardStates[static_cast<SDL_Scancode>(code)] = InputState::UP;
	}
}

void Input::processEvent(const SDL_Event& sdlEvent) {
	// handle newly up keys
	if (sdlEvent.type == SDL_KEYUP) {
		SDL_Scancode code = sdlEvent.key.keysym.scancode;
		if (keyboardStates[code] != InputState::UP) {
			keyboardStates[code] = InputState::NEWLY_UP;
			newlyUpKeycodes.emplace(code);
		}
	}
	// handle newly down keys
	else if (sdlEvent.type == SDL_KEYDOWN) {
		SDL_Scancode code = sdlEvent.key.keysym.scancode;
		if (keyboardStates[code] != InputState::DOWN) {
			keyboardStates[code] = InputState::NEWLY_DOWN;
			newlyDownKeycodes.emplace(code);
		}
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
}

bool Input::getKey(const std::string& code) {
	auto it = keycodeToScancode.find(code);
	if (it != keycodeToScancode.end()) {
		SDL_Scancode scancode = it->second;
		return keyboardStates[scancode] == InputState::DOWN || keyboardStates[scancode] == InputState::NEWLY_DOWN || getKeyDown(code);
	}
	return false;
}

bool Input::getKeyDown(const std::string& code) {
	auto it = keycodeToScancode.find(code);
	if (it != keycodeToScancode.end()) {
		SDL_Scancode scancode = it->second;
		return newlyDownKeycodes.find(scancode) != newlyDownKeycodes.end();
	}
	return false;
}

bool Input::getKeyUp(const std::string& code) {
	auto it = keycodeToScancode.find(code);
	if (it != keycodeToScancode.end()) {
		SDL_Scancode scancode = it->second;
		return newlyUpKeycodes.find(scancode) != newlyUpKeycodes.end();
	}
	return false;
}