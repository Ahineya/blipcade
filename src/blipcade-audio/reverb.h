// Reverb.hpp
#ifndef REVERB_HPP
#define REVERB_HPP

#include "effect.h"
#include <vector>
#include <cstring> // For memset


namespace blipcade::audio {
    class Reverb : public Effect {
    public:
        Reverb(float mix = 0.3f, float feedback = 0.5f);
        ~Reverb();

        float ProcessSample(float inputSample) override;

    private:
        struct DelayLine {
            float* buffer;
            int size;
            int writeIndex;
            float feedback;
            float filterState;

            DelayLine(float delaySeconds, float feedbackFactor)
                : size(static_cast<int>(delaySeconds * SAMPLE_RATE)),
                  writeIndex(0),
                  feedback(feedbackFactor),
                  filterState(0.0f) {
                buffer = new float[size];
                std::memset(buffer, 0, sizeof(float) * size);
            }

            ~DelayLine() {
                delete[] buffer;
            }
        };

        std::vector<DelayLine*> delayLines;
        float mixLevel;
    };
}
#endif // REVERB_HPP