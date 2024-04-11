// std library
#include <string>

// my code
#include "CppComponent.h"

// SDL
#include "SDL2/SDL.h"

// lua
#include "lua/lua.hpp"
#include "LuaBridge/LuaBridge.h"

#pragma once
class UIRenderer : public CppComponent {
public:
	std::string sprite;
	SDL_Color tint = {255, 255, 255, 255};
	int sortingOrder = 0;

	UIRenderer(std::string sprite, SDL_Color tint, int sortingOrder) 
		: sprite(sprite), tint(tint), sortingOrder(sortingOrder) {};
};