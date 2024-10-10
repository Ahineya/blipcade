class SoundSystem {
    init() {
    }
    update(deltaTime) {
        ECS.forEachEntity(['Sound'], (entity, soundComponent) => {
            for (const soundKey in soundComponent.sounds) {
                const soundData = soundComponent.sounds[soundKey];

                if (soundData.isPlaying) {
                    Sound.playSound(soundData.soundId);
                    soundData.isPlaying = false;
                } else if (soundData.isStopping) {
                    Sound.stopSound(soundData.soundId);
                    soundData.isStopping = false;
                }
            }
        });

        ECS.forEachEntity(['ToggleSound'], (entity, toggleSoundComponent) => {
            // Sound.toggleSound(toggleSoundComponent.soundId);
            if (toggleSoundComponent.shouldToggle && !toggleSoundComponent.isPlaying) {
                Sound.playSound(toggleSoundComponent.soundId);
                Sound.setSoundVolume(toggleSoundComponent.soundId, toggleSoundComponent.volume);
                toggleSoundComponent.shouldToggle = false;
                toggleSoundComponent.isPlaying = true;
            } else if (toggleSoundComponent.shouldToggle && toggleSoundComponent.isPlaying) {
                Sound.stopSound(toggleSoundComponent.soundId);
                toggleSoundComponent.shouldToggle = false;
                toggleSoundComponent.isPlaying = false;
            }
        });

        ECS.forEachEntity(['Music'], (entity, musicComponent) => {
            Sound.playSound(musicComponent.musicId);
            Sound.setSoundVolume(musicComponent.musicId, 0.5);
        });
    }
}

export const soundSystem = new SoundSystem();