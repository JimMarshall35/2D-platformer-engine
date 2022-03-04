#pragma once
#include <mutex>
#include <vector>
#include <mutex>
#include <map>
#include <queue>

template<typename T>
class AudioRingBuffer {
    static_assert(std::is_arithmetic<T>::value == true, "must be arithmetic");
public:
    AudioRingBuffer() {}

    AudioRingBuffer(size_t size) {
        Resize(size);
    }

    void Resize(size_t size) {
        _buffer.resize(size);
        _size = size;
    }

    void ReadBlock(T* output, size_t numvalues) {
        std::lock_guard<std::mutex> lock(_bufferMutex);
        for (size_t i = 0; i < numvalues; i++) {
            // read into the output buffer
            output[i] = _buffer[_readptr];
            // set value to zero after reading
            _buffer[_readptr++] = static_cast<T>(0);
            // wrap around if necessary on a per sample basis
            if (_readptr == _size) _readptr = 0;
        }

        _writeptr = _readptr;
    }

    void WriteBlock(T* input, size_t numvalues, double volume) {
        std::lock_guard<std::mutex> lock(_bufferMutex);
        for (size_t i = 0; i < numvalues; i++) {
            _buffer[_writeptr++] += (input[i] * volume);
            if (_writeptr == _size) _writeptr = 0;
        }
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