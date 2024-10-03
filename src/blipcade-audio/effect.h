//
// Created by Pavlo Yevsehnieiev
//

// Effect.hpp
#ifndef EFFECT_HPP
#define EFFECT_HPP

constexpr int SAMPLE_RATE = 44100; // TODO: Get this from the audio engine

class Effect {
public:
    virtual ~Effect() = default;
    // Process a single sample. Should be overridden by derived effects.
    virtual float ProcessSample(float inputSample) = 0;
};

#endif // EFFECT_HPP#endif //EFFECT_H
