#pragma once

// std library
#include <string>

// my code
#include "CppComponent.h"
#include "../errors/Error.h"
#include "../utils/LuaStateSaver.h"

// SDL
#include "SDL2/SDL.h"

// lua
#include "lua/lua.hpp"
#include "LuaBridge/LuaBridge.h"

class Actor;

// MUST include a Transform component
class UIRenderer : public CppComponent {
public:
	std::string sprite = "";
	SDL_Color tint = {255, 255, 255, 255};
	int sortingOrder = 0;

	// starts nil
	luabridge::LuaRef transform = luabridge::LuaRef(LuaStateSaver::luaState);

	UIRenderer(std::string sprite, SDL_Color tint, int sortingOrder) 
		: sprite(sprite), tint(tint), sortingOrder(sortingOrder) {};

	void onStart();

	void onUpdate();

	void setSprite(std::string& sprite) {
		this->sprite = sprite;
	}

	std::string& getSprite() {
		return this->sprite;
	}

	void setTint(SDL_Color& tint) {
		this->tint = tint;
	}

	SDL_Color& getTint() {
		return this->tint;
	}

	void setSortingOrder(int sortingOrder) {
		this->sortingOrder = sortingOrder;
	}

	int getSortingOrder() {
		return this->sortingOrder;
	}
};