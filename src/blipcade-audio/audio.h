//
// Created by Pavlo Yevsehnieiev
//

#ifndef AUDIO_H
#define AUDIO_H
#include <raylib.h>
#include <unordered_map>
#include <vector>


namespace blipcade::audio {
    using SoundHandle = uint32_t;

    class Audio {
    public:
        Audio();

        ~Audio();

        SoundHandle LoadSound(const std::string &path);

        void PlaySound(SoundHandle sound) const;

        void ToggleSound(SoundHandle sound) const;

        void StopSound(SoundHandle sound) const;

        void SetSoundVolume(SoundHandle sound, float volume) const;

        AudioStream musicStream;

    private:
        std::unordered_map<SoundHandle, ::Sound> sounds;
    };
} // audio
// blipcade

#endif //AUDIO_H
