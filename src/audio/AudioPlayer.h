#pragma once

// std library
#include <string>

// my code
#include "../utils/ResourceManager.h"

class AudioPlayer
{
public:
	ResourceManager& resourceManager;

	AudioPlayer(ResourceManager& resourceManager) : resourceManager(resourceManager) {}

	// play a sound "loops" times
	void play(std::string& soundName, int loops, int channel);
};
