//
// Created by Pavlo Yevsehnieiev
//

#ifndef ECHO_H
#define ECHO_H
#include "effect.h"
#include <vector>
#include <cstring> // For memset

namespace blipcade {
    namespace audio {
        class Echo : public Effect {
        public:
            Echo(float delaySeconds = 0.3f, float feedback = 0.5f, float mix = 0.3f);

            ~Echo();

            float ProcessSample(float inputSample) override;

            void AudioProcessEffectDelay(void *buffer, unsigned int frames);

        private:
            struct DelayLineStruct {
                float *buffer;
                int size;
                int writeIndex;
                float feedback;

                DelayLineStruct(float delaySeconds, float feedbackFactor)
                    : size(static_cast<int>(delaySeconds * SAMPLE_RATE)),
                      writeIndex(0),
                      feedback(feedbackFactor) {
                    buffer = new float[size];
                    std::memset(buffer, 0, sizeof(float) * size);
                }

                ~DelayLineStruct() {
                    delete[] buffer;
                }
            };

            std::vector<DelayLineStruct *> delayLines;
            float mixLevel;
            float feedbackFactor;



            float *delayBuffer = nullptr;
            unsigned int delayBufferSize = 0;
        };
    }
} // blipcade

#endif //ECHO_H
