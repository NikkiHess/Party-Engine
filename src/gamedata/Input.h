#pragma once

// std library
#include <unordered_map>
#include <unordered_set>

// dependencies
#include <SDL2/sdl.h>

enum InputState {
	UP,
	NEWLY_UP,
	DOWN,
	NEWLY_DOWN
};

class Input {
public:
	// initializes all inputs begin in the up state
	Input() {
		for (int code = SDL_SCANCODE_UNKNOWN; code < SDL_NUM_SCANCODES; ++code) {
			keyboardStates[static_cast<SDL_Scancode>(code)] = InputState::UP;
		}
	}

	// call every frame at the start of event loop
	void processEvent(const SDL_Event& sdlEvent);

	// call at the end of every frame
	void lateUpdate();

	// returns whether the key is held
	bool getKey(SDL_Scancode keycode);
	
	// returns whether the key is newly up
	bool getKeyUp(SDL_Scancode keycode);

	// returns whether the key is newly down
	bool getKeyDown(SDL_Scancode keycode);
private:
	std::unordered_map<SDL_Scancode, InputState> keyboardStates;
	std::unordered_set<SDL_Scancode> newlyUpKeycodes;
	std::unordered_set<SDL_Scancode> newlyDownKeycodes;
};