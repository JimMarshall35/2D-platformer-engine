#include "PortAudioPlayer.h"
#include <iostream>
#include "audiofile.h"


AudioRingBuffer<float> PortAudioPlayer::s_soundFxBuffer(SOUNDFX_BUFFER_SIZE_SAMPLES);

PortAudioPlayer::PortAudioPlayer()
{
	auto err = Pa_Initialize();
	if (err != paNoError) {
		std::cout << "port audio initialization error" << std::endl;
	}
    PaStream* stream;
    err = Pa_OpenDefaultStream(
        &stream,
        0,          /* no input channels */
        NUMCHANNELS,          /* stereo output */
        paFloat32,  /* 32 bit floating point output */
        SAMPLE_RATE,
        paFramesPerBufferUnspecified,        /* frames per buffer, i.e. the number of sample frames that PortAudio will request from the callback. Many apps may want to use paFramesPerBufferUnspecified, which tells PortAudio to pick the best, possibly changing, buffer size.*/
        paSoundFxCallback, /* this is your callback function */
        nullptr /*This is a pointer that will be passed to your callback*/ 
    ); 
    if (err != paNoError) {
        std::cout << "Pa_OpenDefaultStream error" << std::endl;
    }
    err = Pa_StartStream(stream);
    if (err != paNoError) {
        std::cout << "Pa_StartStream error" << std::endl;
    }
    //AudioClipID id = LoadClip("message.wav");
    //PlayClip(id);
}

AudioClipID PortAudioPlayer::LoadClip(std::string path)
{
    // get a unique id for the clip
    auto id = _idGenerator.GetId();

    // load from file
    AudioFile<float> af;
    af.load(path);
    assert(af.getSampleRate() == SAMPLE_RATE);
    int numSamples = af.getNumSamplesPerChannel();

    // if source is mono load the same sample into left and right channels
    int secondInterleafChannel = 1;
    if (af.isMono())
        secondInterleafChannel = 0;

    // interleaf the left and right channels and store in _audioDataMap
    std::vector<float> samples;
    samples.resize(numSamples * 2);
    for (int i = 0; i < numSamples; i++) {
        samples[i * 2] = af.samples[0][i];
        samples[(i * 2) + 1] = af.samples[secondInterleafChannel][i];
    }
    _audioDataMap.emplace(SamplesIdPair(id, samples));

	return id;
}

bool PortAudioPlayer::PlayClip(AudioClipID id)
{
    if (_audioDataMap.find(id) == _audioDataMap.end()) {
        return false;
    }
    s_soundFxBuffer.WriteBlock(_audioDataMap[id].data(), _audioDataMap[id].size());
    return true;
}

bool PortAudioPlayer::DeleteClip(AudioClipID id)
{
    if (_audioDataMap.find(id) == _audioDataMap.end()) {
        return false;
    }
    _audioDataMap.erase(id);
    _idGenerator.DeleteId(id);
    return true;
}

int PortAudioPlayer::paSoundFxCallback(const void* inputBuffer, void* outputBuffer, unsigned long framesPerBuffer, const PaStreamCallbackTimeInfo* timeInfo, PaStreamCallbackFlags statusFlags, void* userData)
{
    float* out = (float*)outputBuffer;

    s_soundFxBuffer.ReadBlock(out, framesPerBuffer * NUMCHANNELS);

    return 0;
}

bool PortAudioPlayer::PlayClip(AudioClipID id, float volume)
{
    if (_audioDataMap.find(id) == _audioDataMap.end()) {
        return false;
    }
    s_soundFxBuffer.WriteBlock(_audioDataMap[id].data(), _audioDataMap[id].size(), volume);
    return true;
}

