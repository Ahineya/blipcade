//
// Created by Pavlo Yevsehnieiev
//

#include "audio.h"

// It is definitely the most basic v1.0 implementation of the Audio class.
namespace blipcade::audio {
    static int soundCount = 0;

    Audio::Audio() = default;
    Audio::~Audio() = default;

    SoundHandle Audio::LoadSound(const std::string& path) {
        auto sound = ::LoadSound(path.c_str());
        sounds.insert(std::make_pair(soundCount, sound));
        return soundCount++;
    }

    void Audio::PlaySound(const SoundHandle sound) const {
        if (!::IsSoundPlaying(sounds.at(sound)))
        ::PlaySound(sounds.at(sound));
    }

    void Audio::StopSound(const SoundHandle sound) const {
        ::StopSound(sounds.at(sound));
    }

    void Audio::SetSoundVolume(const SoundHandle sound, const float volume) const {
        ::SetSoundVolume(sounds.at(sound), volume);
    }
} // audio
// blipcade