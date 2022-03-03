#pragma once
#include "IAudioPlayer.h"
#include "portaudio.h"
#include <vector>
#include <mutex>
#include <map>
#include <queue>

template<typename T>
class AudioRingBuffer {
    static_assert(std::is_arithmetic<T>::value == true, "must be arithmetic");
public:
    AudioRingBuffer(){}

    AudioRingBuffer(size_t size){
        Resize(size);
    }

    void Resize(size_t size) {
        _buffer.resize(size);
        _size = size;
    }

    void ReadBlock(T* output, size_t numvalues) {
        std::lock_guard<std::mutex> lock(_bufferMutex);
        for (size_t i = 0; i < numvalues; i++) {
            output[i] = _buffer[_readptr];
            _buffer[_readptr++] = static_cast<T>(0);
            if (_readptr == _size) _readptr = 0;
        }
        _writeptr = _readptr;
    }

    void WriteBlock(T* input, size_t numvalues) {
        std::lock_guard<std::mutex> lock(_bufferMutex);
        for (size_t i = 0; i < numvalues; i++) {
            _buffer[_writeptr++] += input[i];
            if (_writeptr == _size) _writeptr = 0;
        }
    }

private:
    std::mutex _bufferMutex;
    std::vector<T> _buffer;
    size_t _size;
    size_t _readptr = 0;
    size_t _writeptr = 0;
};


typedef struct
{
    float left_phase;
    float right_phase;
}paTestData;

class AudioClipIdGenerator {
public:
    AudioClipID GetClipId();
    void DeleteId(AudioClipID id);
private:
    std::queue<AudioClipID> _deletedClipIds;
    AudioClipID _onId = 0;
};
using AudioSamplesMap = std::map<AudioClipID, std::vector<float>>;
using SamplesIdPair = std::pair<AudioClipID, std::vector<float>>;
class PortAudioPlayer : public IAudioPlayer
{
public:
    PortAudioPlayer();

    // Inherited via IAudioPlayer
    virtual AudioClipID LoadSound(std::string path) override;
    virtual bool PlaySound(AudioClipID id) override;
    virtual bool DeleteClip(AudioClipID id) override;
private:
    static int paSoundFxCallback(const void* inputBuffer, void* outputBuffer,
        unsigned long framesPerBuffer,
        const PaStreamCallbackTimeInfo* timeInfo,
        PaStreamCallbackFlags statusFlags,
        void* userData);
    
private:
    static AudioRingBuffer<float> s_soundFxBuffer;
    AudioSamplesMap _audioDataMap;
    AudioClipIdGenerator _idGenerator;
    
};

