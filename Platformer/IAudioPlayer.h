#pragma once
#include <string>
using AudioClipID = unsigned int;
class IAudioPlayer {
public:
	virtual AudioClipID LoadClip(std::string path) = 0;
	virtual bool PlayClip(AudioClipID id) = 0;
	virtual bool PlayClip(AudioClipID id, float volume) = 0;
	virtual bool DeleteClip(AudioClipID id) = 0;
};