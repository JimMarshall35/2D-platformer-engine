#pragma once
#include "IAudioPlayer.h"
#include "portaudio.h"
#include "AudioRingBuffer.h"
#include "IdGenerator.h"

#define SAMPLE_RATE 44100
#define SOUNDFX_BUFFER_SIZE_SECONDS 5
#define NUMCHANNELS 2
#define SOUNDFX_BUFFER_SIZE_SAMPLES SOUNDFX_BUFFER_SIZE_SECONDS * SAMPLE_RATE * NUMCHANNELS


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
    static int paSoundFxCallback(const void* inputBuffer, 
                                 void* outputBuffer,
                                 unsigned long framesPerBuffer,
                                 const PaStreamCallbackTimeInfo* timeInfo,
                                 PaStreamCallbackFlags statusFlags,
                                 void* userData);
    
private:
    static AudioRingBuffer<float> s_soundFxBuffer;
    AudioSamplesMap _audioDataMap;
    IdGenerator<AudioClipID> _idGenerator;
    

    // Inherited via IAudioPlayer
    virtual bool PlayClip(AudioClipID id, float volume) override;

};

