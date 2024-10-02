class SoundSystem {
    init() {

        const musicEntity = ECS.createEntity();
        ECS.addComponent(musicEntity, 'Music', {
            musicId: Sound.loadSound('resources/music-1.mp3')
        });
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

        ECS.forEachEntity(['Music'], (entity, musicComponent) => {
            Sound.playSound(musicComponent.musicId);
            Sound.setSoundVolume(musicComponent.musicId, 0.5);
        });
    }
}

export const soundSystem = new SoundSystem();