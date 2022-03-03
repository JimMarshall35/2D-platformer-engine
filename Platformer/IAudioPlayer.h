#pragma once
#include <string>
using AudioClipID = unsigned int;
class IAudioPlayer {
public:
	virtual AudioClipID LoadSound(std::string path) = 0;
	virtual bool PlaySound(AudioClipID id) = 0;
	virtual bool DeleteClip(AudioClipID id) = 0;
};