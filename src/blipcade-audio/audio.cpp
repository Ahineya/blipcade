//
// Created by Pavlo Yevsehnieiev
//

#include "audio.h"

#include <echo.h>
#include <iostream>
#include <reverb.h>

// It is definitely the most basic v1.0 implementation of the Audio class.
namespace blipcade::audio {
    static int soundCount = 0;
    Reverb reverb(1.0f, 0.1f);
    Echo echo(0.1f, 0.5f, 0.5f);

    void generateSamples(void *buffer, unsigned int frames) {
        float *d = (float *) buffer;

        for (unsigned int i = 0; i < frames; i++) {
            // Make this sine wave louder
            // d[i] = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
            d[i] = d[i] * 0.1f;
        }
    }

    // Cycles per second (hz)
    float frequency = 440.0f;

    // Audio frequency, for smoothing
    float audioFrequency = 440.0f;

    // Previous value, used to test if sine needs to be rewritten, and to smoothly modulate frequency
    float oldFrequency = 1.0f;

    // Index for audio rendering
    float sineIdx = 0.0f;

    // Audio input processing callback
    void AudioInputCallback(void *buffer, unsigned int frames) {
        audioFrequency = frequency + (audioFrequency - frequency) * 0.95f;
        float incr = audioFrequency / 48000.0f;

        // Stereo sine
        for (unsigned int i = 0; i < frames * 2; i += 2) {
            ((float *) buffer)[i] = sinf(2 * PI * sineIdx); // Range -1.0f to 1.0f
            ((float *) buffer)[i + 1] = sinf(2 * PI * sineIdx); // Range -1.0f to 1.0f

            sineIdx += incr;
            if (sineIdx > 1.0f) sineIdx -= 1.0f;
        }

        // Instead of stereo sine, this crap should be a mixer.
    }

    static float *delayBuffer = NULL;
    static unsigned int delayBufferSize = 0;
    static unsigned int delayReadIndex = 2;
    static unsigned int delayWriteIndex = 0;
    // Allocate buffer for the delay effect

    static void AudioProcessEffectDelay(void *buffer, unsigned int frames) {
        for (unsigned int i = 0; i < frames * 2; i += 2) {
            float leftDelay = delayBuffer[delayReadIndex++]; // ERROR: Reading buffer -> WHY??? Maybe thread related???
            float rightDelay = delayBuffer[delayReadIndex++];

            if (delayReadIndex == delayBufferSize) delayReadIndex = 0;

            ((float *) buffer)[i] = 0.5f * ((float *) buffer)[i] + 0.5f * leftDelay;
            ((float *) buffer)[i + 1] = 0.5f * ((float *) buffer)[i + 1] + 0.5f * rightDelay;

            delayBuffer[delayWriteIndex++] = ((float *) buffer)[i];
            delayBuffer[delayWriteIndex++] = ((float *) buffer)[i + 1];
            if (delayWriteIndex == delayBufferSize) delayWriteIndex = 0;
        }
    }

    static Echo echoEff(0.1f, 0.5f, 0.5f);

    static void EffectChain(void *buffer, unsigned int frames) {
        // Process audio buffer with reverb effect
        // reverb.ProcessAudioEffect(buffer, frames);

        // Process audio buffer with echo effect
        // echo.ProcessAudioEffect(buffer, frames);

        echoEff.AudioProcessEffectDelay(buffer, frames);
    }

    Audio::Audio() {
        // delayBufferSize = 48000 * 2; // 1 second delay (device sampleRate*channels)
        // delayBuffer = (float *) RL_CALLOC(delayBufferSize, sizeof(float));

        // AudioStream fuck = LoadAudioStream(48000, 32, 2);

        // auto wave = ::LoadWave("resources/music1.mp3");

        // SetAudioStreamCallback(fuck, AudioInputCallback);

        // PlayAudioStream(fuck);

        // Music music = LoadMusicStream("resources/music-1.mp3");
        // PlayMusicStream(music);
        // AttachAudioStreamProcessor(music.stream, AudioProcessEffectDelay); // This works

        // AttachAudioStreamProcessor(music.stream, EffectChain); // This does not

        // musicStream = fuck;
    }

    Audio::~Audio() {
        std::cout << "Audio destructor" << std::endl;
        // CloseAudioStream(stream);
    }

    SoundHandle Audio::LoadSound(const std::string &path) {
        auto sound = ::LoadSound(path.c_str());

        // AttachAudioStreamProcessor(sound.stream, AudioProcessEffectDelay);

        // unsigned int sampleSize;    // Bit depth (bits per sample): 8, 16, 32 (24 not supported)

        std::cout << "Sample rate: " << sound.stream.sampleRate << std::endl;
        std::cout << "Sample size: " << sound.stream.sampleSize << std::endl;

        sounds.insert(std::make_pair(soundCount, sound));
        return soundCount++;
    }

    void Audio::PlaySound(const SoundHandle sound) const {
        if (!::IsSoundPlaying(sounds.at(sound))) {
            ::PlaySound(sounds.at(sound));
        }
    }

    void Audio::ToggleSound(const SoundHandle sound) const {
        if (::IsSoundPlaying(sounds.at(sound))) {
            ::StopSound(sounds.at(sound));
        } else {
            ::PlaySound(sounds.at(sound));
        }
    }

    void Audio::StopSound(const SoundHandle sound) const {
        ::StopSound(sounds.at(sound));
    }

    void Audio::SetSoundVolume(const SoundHandle sound, const float volume) const {
        ::SetSoundVolume(sounds.at(sound), volume);
    }
} // audio
// blipcade
