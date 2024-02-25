#include "Input.h"

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

bool Input::getKey(SDL_Scancode code) {
	return keyboardStates[code] == InputState::DOWN || keyboardStates[code] == InputState::NEWLY_DOWN;
}

bool Input::getKeyDown(SDL_Scancode code) {
	return newlyDownKeycodes.find(code) != newlyDownKeycodes.end();
}

bool Input::getKeyUp(SDL_Scancode code) {
	return newlyUpKeycodes.find(code) != newlyUpKeycodes.end();
}