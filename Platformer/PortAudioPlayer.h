#pragma once
#include "IAudioPlayer.h"
#include "portaudio.h"
#include "AudioRingBuffer.h"
#include "IdGenerator.h"



typedef struct
{
    float left_phase;
    float right_phase;
}paTestData;


using AudioSamplesMap = std::map<AudioClipID, std::vector<float>>;
using SamplesIdPair = std::pair<AudioClipID, std::vector<float>>;
class PortAudioPlayer : public IAudioPlayer
{
public:
    PortAudioPlayer();

    // Inherited via IAudioPlayer
    virtual AudioClipID LoadClip(std::string path) override;
    virtual bool DeleteClip(AudioClipID id) override;
    virtual bool PlayClip(AudioClipID id) override;

private:
    static int paSoundFxCallback(const void* inputBuffer, void* outputBuffer,
        unsigned long framesPerBuffer,
        const PaStreamCallbackTimeInfo* timeInfo,
        PaStreamCallbackFlags statusFlags,
        void* userData);
    
private:
    static AudioRingBuffer<float> s_soundFxBuffer;
    AudioSamplesMap _audioDataMap;
    IdGenerator<AudioClipID> _idGenerator;
    
};

