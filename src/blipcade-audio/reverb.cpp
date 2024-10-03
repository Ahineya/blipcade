//
// Created by Pavlo Yevsehnieiev
//

#include "reverb.h"


namespace blipcade::audio {

    Reverb::Reverb(float mix, float feedback)
        : mixLevel(mix) {
        // Initialize delay lines with slightly different delay times for a natural reverb
        float delays[4] = {0.050f, 0.056f, 0.061f, 0.068f}; // in seconds
        for (int i = 0; i < 4; ++i) {
            delayLines.emplace_back(new DelayLine(delays[i], feedback));
        }
    }

    Reverb::~Reverb() {
        for(auto& dl : delayLines) {
            delete dl;
        }
    }

    float Reverb::ProcessSample(float inputSample) {
        float reverbSample = 0.0f;

        for(auto& dl : delayLines) {
            // Read delayed sample
            float delayedSample = dl->buffer[dl->writeIndex];

            // Apply a simple one-pole low-pass filter
            float alpha = 0.5f; // Adjust for cutoff frequency
            dl->filterState = alpha * delayedSample + (1.0f - alpha) * dl->filterState;
            float filteredSample = dl->filterState;

            // Accumulate reverb samples
            reverbSample += filteredSample;

            // Write new sample with feedback
            dl->buffer[dl->writeIndex] = inputSample + filteredSample * dl->feedback;

            // Increment and wrap write index
            dl->writeIndex = (dl->writeIndex + 1) % dl->size;
        }

        // Mix original and reverb signals
        float outputSample = inputSample + reverbSample * mixLevel;

        // Clamp to prevent clipping
        if(outputSample > 1.0f) outputSample = 1.0f;
        if(outputSample < -1.0f) outputSample = -1.0f;

        return outputSample;
    }

} // audio
// blipcade