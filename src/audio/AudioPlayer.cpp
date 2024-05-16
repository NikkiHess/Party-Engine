// std library
#include <string>
#include <cstdlib>

// my code
#include "../errors/Error.h"
#include "AudioPlayer.h"

// dependencies
#include "AudioHelper.h"

//#define WSL 0

void AudioPlayer::play(const int channel, const std::string& soundName, const bool doesLoop) {
	// suppress unused variable warnings in the case that WSL is defined
	(void)channel;
	(void)soundName;
	(void)doesLoop;

// this code will not compile if WSL is defined above
// make WSL=1
#if !defined(WSL) || (WSL == 0)
	std::string soundPath = "resources/audio/" + soundName; // the audio's path
	// Verify that the intro music exists
	if (resourceManager->fileExists(soundPath + ".wav")) {
		soundPath += ".wav";
	}
	else if (resourceManager->fileExists(soundPath + ".ogg")) {
		soundPath += ".ogg";
	}
	if (soundPath == "resources/audio/" + soundName) {
		Error::error("failed to play audio clip " + soundName);
	}

	// the sound to be played
	Mix_Chunk* sound = nullptr;

	// is the sound cached? Load it
	auto it = resourceManager->sounds.find(soundName);
	if (it != resourceManager->sounds.end()) {
		sound = it->second;
	}
	// otherwise, load from path and cache it
	else {
		sound = AudioHelper::Mix_LoadWAV498(soundPath.c_str());
		resourceManager->sounds[soundName] = sound;
	}

	// play the sound on the specified channel, with -1 loops if we're looping / 0 otherwise
	AudioHelper::Mix_PlayChannel498(channel, sound, doesLoop ? -1 : 0);
#endif
}

void AudioPlayer::halt(const int channel) {
	AudioHelper::Mix_HaltChannel498(channel);
}

void AudioPlayer::setVolume(const int channel, const int volume) {
	AudioHelper::Mix_Volume498(channel, volume);
}