#pragma once
#include <portaudio.h>
#include "audio_pipeline.h"


class PortaudioInputSource : public IAudioSource {
public:
	PortaudioInputSource();
	~PortaudioInputSource();
	void Start() override;
	void Stop() override;
	AudioData GetAudioData() override;
private:
    const size_t preferredBufferFrames = 480; // Adjust based on your needs

    static int RecordCallback(
        const void* inputBuffer,
        void* outputBuffer,
        unsigned long framesPerBuffer,
        const PaStreamCallbackTimeInfo* timeInfo,
        PaStreamCallbackFlags statusFlags,
        void* userData);

	PaStream* stream;
    std::vector<float> ringBuffer;
    size_t ringBufferSize;
    size_t writePos;
    size_t readPos;
    std::mutex bufferMutex;
    unsigned int deviceSampleRate;
    unsigned int deviceChannels;
};
