#pragma once

// std library
#include <string>

// my code
#include "../utils/ResourceManager.h"

class AudioPlayer
{
public:
	static inline ResourceManager* resourceManager;

	// play a sound on the specified channel, looping if doesLoop is true
	static void play(const int channel, const std::string& soundName, const bool doesLoop);

	// halts all audio on the specified channel
	static void halt(const int channel);

	// sets the volume (volume->int)
	// volume range 0-100
	static void setVolume(const int channel, const float volume);
};
