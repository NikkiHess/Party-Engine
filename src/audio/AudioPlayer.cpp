// std library
#include <string>
#include <cstdlib>

// my code
#include "../errors/Error.h"
#include "AudioPlayer.h"

// dependencies
#include "AudioHelper.h"

#define WSL 0

void AudioPlayer::play(std::string& soundName, int loops, int channel) {
// this code will not compile if WSL is defined above
#if not(defined(WSL) && WSL == 1)
	std::string soundPath = "resources/audio/" + soundName; // the audio's path
	// Verify that the intro music exists
	if (resourceManager.fileExists(soundPath + ".wav")) {
		soundPath += ".wav";
	}
	else if (resourceManager.fileExists(soundPath + ".ogg")) {
		soundPath += ".ogg";
	}
	if (soundPath == "resources/audio/" + soundName) {
		Error::error("failed to play audio clip " + soundName);
	}

	// the sound to be played
	Mix_Chunk* sound = nullptr;

	// Is the sound cached? Load it
	auto it = resourceManager.sounds.find(soundName);
	if (it != resourceManager.sounds.end()) {
		sound = it->second;
	}
	// Otherwise, load from path and cache it
	else {
		sound = AudioHelper::Mix_LoadWAV498(soundPath.c_str());
		resourceManager.sounds[soundName] = sound;
	}

	// Play the sound on channel 0, looping indefinitely
	AudioHelper::Mix_PlayChannel498(channel, sound, loops);
#endif
}