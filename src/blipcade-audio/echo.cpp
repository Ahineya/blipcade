//
// Created by Pavlo Yevsehnieiev
//

#include "echo.h"

#include <raylib.h>

namespace blipcade {
namespace audio {

    Echo::Echo(float delaySeconds, float feedback, float mix)
    : mixLevel(mix), feedbackFactor(feedback) {
        // Initialize delay lines with slightly different delay times for a richer echo
        float delays[2] = {delaySeconds, delaySeconds + 0.015f}; // in seconds
        for(int i = 0; i < 2; ++i) {
            delayLines.emplace_back(new DelayLineStruct(delays[i], feedbackFactor));
        }

        delayBufferSize = 48000*2;      // 1 second delay (device sampleRate*channels)
        delayBuffer = (float *)RL_CALLOC(delayBufferSize, sizeof(float));
    }

    Echo::~Echo() {
        for(auto& dl : delayLines) {
            delete dl;
        }
    }

    float Echo::ProcessSample(float inputSample) {

    }


    unsigned int delayReadIndex = 2;
    unsigned int delayWriteIndex = 0;

    void Echo::AudioProcessEffectDelay(void *buffer, unsigned int frames)
    {
        for (unsigned int i = 0; i < frames*2; i += 2)
        {
            float leftDelay = delayBuffer[delayReadIndex++];    // ERROR: Reading buffer -> WHY??? Maybe thread related???
            float rightDelay = delayBuffer[delayReadIndex++];

            if (delayReadIndex == delayBufferSize) delayReadIndex = 0;

            ((float *)buffer)[i] = 0.5f*((float *)buffer)[i] + 0.5f*leftDelay;
            ((float *)buffer)[i + 1] = 0.5f*((float *)buffer)[i + 1] + 0.5f*rightDelay;

            delayBuffer[delayWriteIndex++] = ((float *)buffer)[i];
            delayBuffer[delayWriteIndex++] = ((float *)buffer)[i + 1];
            if (delayWriteIndex == delayBufferSize) delayWriteIndex = 0;
        }
    }
} // audio
} // blipcade