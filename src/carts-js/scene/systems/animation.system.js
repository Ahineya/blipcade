class AnimationSystem {
    init() {

    }

    update(deltaTime) {
        ECS.forEachEntity(["Animation", "Sprite"], (_, animation, sprite) => {
            animation.frameTime += deltaTime; // delta time in ms

            if (animation.frameTime >= animation.frameDuration) {
                animation.frameTime = 0;
                animation.currentFrame++;

                if (animation.currentFrame >= animation.animations[animation.currentAnimation].length) {
                    animation.currentFrame = 0;
                }
            }

            sprite.spriteIndex = animation.animations[animation.currentAnimation][animation.currentFrame];
            sprite.spriteSheet = animation.spritesheets[animation.currentAnimation];
        });
    }
}

export const animationSystem = new AnimationSystem();