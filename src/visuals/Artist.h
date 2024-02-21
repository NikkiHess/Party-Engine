#pragma once

// std library
#include <string>

// my code
#include "Camera.h"
#include "../gamedata/Actor.h"
#include "../utils/config/ConfigManager.h"
#include "../gamedata/GameInfo.h"
#include "../utils/ResourceManager.h"

// dependencies
#include "glm/glm.hpp"
#include "SDL2/SDL_image.h"
#include "SDL2/SDL_render.h"
#include "SDL2/SDL_ttf.h"

// handles drawing of visuals (not displaying, that's Renderer's job)
class Artist
{
public:
	ConfigManager& configManager; // the ConfigManager passed in from Renderer
	SDL_Renderer* sdlRenderer = nullptr; // the SDL renderer object passed in from Renderer
	ResourceManager& resourceManager;

	Artist(ConfigManager& configManager, ResourceManager& resourceManager) : configManager(configManager), resourceManager(resourceManager) {}

	// draw an unchanging image on the screen
	void drawUIImage(std::string& imageName, glm::ivec2 pos, glm::ivec2 size);

	// draw an unchanging image on the screen
	void drawActor(Actor& actor, Camera& camera);

	// draw an Actor's box collider on the screen
	void drawBoxCollider(Actor& actor, glm::vec2& actorScreenPos, glm::vec2& pivot);

	// draw an Actor's box trigger on the screen
	void drawBoxTrigger(Actor& actor, glm::vec2& actorScreenPos, glm::vec2& pivot);

	// draw text on the screen
	void drawUIText(std::string& text, SDL_Color fontColor, glm::ivec2 pos);
};

