import {getScale} from "./movement.system.js";
import {MiasmaParticle, Particle} from "../particles.js";

export const RenderLayer = {
    Background: 1,
    Entities: 2,
    Foreground: 3
}

class DrawSystem {
    renderLayers = {};

    constructor() {
        this.initRenderLayers();
    }

    init() {
    }

    initRenderLayers() {
        this.renderLayers = {
            [RenderLayer.Background]: [],
            [RenderLayer.Entities]: [],
            [RenderLayer.Foreground]: []
        }
    }

    update() {
        this.initRenderLayers();

        // Let's check if we have a scene
        let scene = false;
        ECS.forEachEntity(["Scene"], (entity, sceneComponent) => {
            scene = sceneComponent;
        });

        const componentTypes = ["Render"];
        if (scene) {
            componentTypes.push("Scene");
        }

        ECS.forEachEntity(componentTypes, (entity, render) => {
            const sprite = ECS.getComponent(entity, "Sprite");
            const spriteGroup = ECS.getComponent(entity, "SpriteGroup");

            if (spriteGroup) {
                // Entities with a sprite group
                const sprites = spriteGroup.sprites;

                // Determine the sorting Y position for the entire group
                const x = sprites[0].position.x;
                const y = sprites[0].position.y;

                const o = {
                    entity,
                    sprites,
                    properties: {x, y, renderOrder: render.renderOrder || 0}
                };

                this.renderLayers[render.layer].push(o);
            } else if (sprite) {
                // Entities with sprite
                const {position: {x, y}, spriteIndex, spriteSheet, size: {width, height}, flipX, origin: {x: ox, y: oy}} = sprite;
                const layer = render.layer;

                if (!layer) {
                    return;
                }

                const player = ECS.getComponent(entity, "Player");
                const playerScale = ECS.getComponent(entity, "PlayerScale");

                const scale = player ? getScale(y, playerScale.min, playerScale.max, playerScale.quarterScreenMin) : 1;

                const o = {
                    entity,
                    properties: {x, y, width, height, spriteIndex, spriteSheet, flipX, ox, oy, scale, renderOrder: render.renderOrder || 0}
                };

                this.renderLayers[layer].push(o);
            } else {
                // Entities without sprite
                const emitter = ECS.getComponent(entity, "ParticleEmitter");
                const layer = render.layer;

                if (!layer) {
                    return;
                }

                const o = {
                    entity,
                    properties: {x: emitter.position.x, y: emitter.position.y, width: 1, height: 100, ox: 0.5 , oy: 0.5, id: emitter.id}
                };

                this.renderLayers[layer].push(o);
            }
        });

        // Sort entities by origin Y
        this.renderLayers[RenderLayer.Entities].sort((a, b) => {
            return (a.properties.y) - (b.properties.y);
        });
    }

    draw() {



        // Render background
        this.renderLayers[RenderLayer.Background].forEach(({entity, properties: {x, y, spriteIndex, spriteSheet, flipX, ox, oy}}) => {
            Graphics.drawSprite(x, y, spriteIndex, spriteSheet.toString(), flipX);
        });

        // Render entities and player with y-sorting
        this.renderLayers[RenderLayer.Entities].forEach(({entity, sprites, properties}) => {

            if (sprites) {
                sprites.forEach(sprite => {
                    const {
                        position: { x, y },
                        spriteIndex,
                        spriteSheet,
                        flipX,
                        origin: { x: ox, y: oy },
                    } = sprite;

                    Graphics.drawSpriteEx(x, y, spriteIndex, spriteSheet.toString(), flipX, false, 1, ox, oy);
                });

                return;
            }

            if (ECS.getComponent(entity, "Player")) {
                const {x, y, spriteIndex, spriteSheet, flipX, ox, oy, scale} = properties;
                Graphics.drawSpriteEx(x, y, spriteIndex, spriteSheet, flipX, false, scale, ox, oy);
            } else if (ECS.getComponent(entity, "ParticleEmitter")) {
                // Special case for particles
                const {id} = ECS.getComponent(entity, "ParticleEmitter");

                ECS.forEachEntity(["Particle"], (particleEntity, particle) => {
                    if (particle.emitterId !== id) {
                        return;
                    }

                    const {position: {x, y}, size, color, type} = particle;

                    switch (particle.type) {
                        case "MiasmaParticle":
                            Graphics.putPixel(x, y, MiasmaParticle.colors[color] || 0xFA);
                            break;
                        case "FireParticle":
                            Graphics.drawFilledCircle(x, y, size, Particle.gradient[color] || 0xFA);
                            break;
                    }
                });
            } else {
                const {x, y, spriteIndex, spriteSheet, flipX, ox, oy} = properties;
                // Graphics.drawSprite(x, y, spriteIndex, spriteSheet, flipX);
                Graphics.drawSpriteEx(x, y, spriteIndex, spriteSheet.toString(), flipX, false, 1, ox, oy);
            }
        });

        // Render foreground
        this.renderLayers[RenderLayer.Foreground].forEach(({entity, properties: {x, y, spriteIndex, spriteSheet, flipX, ox, oy}}) => {
            Graphics.drawSpriteEx(x, y, spriteIndex, spriteSheet, flipX, false, 1, ox, oy);
        });
    }
}

export const drawSystem = new DrawSystem();