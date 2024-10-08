import {getScale} from "./movement.system.js";
import {MiasmaParticle, Particle} from "../particles.js";

export const RenderLayer = {
    Background: 1,
    Entities: 2,
    Foreground: 3
}

class DrawSystem {
    constructor() {
    }

    init() {
    }

    draw() {

        // We want a y-sorting layer-based rendering system.

        const renderLayers = {
            [RenderLayer.Background]: [],
            [RenderLayer.Entities]: [],
            [RenderLayer.Foreground]: []
        }

        ECS.forEachEntity(["Render"], (entity, render) => {
            const sprite = ECS.getComponent(entity, "Sprite");
            if (sprite) {
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
                    properties: {x, y, width, height, spriteIndex, spriteSheet, flipX, ox, oy, scale}
                };

                renderLayers[layer].push(o);
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

                renderLayers[layer].push(o);
            }
        });

        // Render background
        renderLayers[RenderLayer.Background].forEach(({entity, properties: {x, y, spriteIndex, spriteSheet, flipX, ox, oy}}) => {
            Graphics.drawSprite(x, y, spriteIndex, spriteSheet, flipX);
        });

        // Sort entities by origin Y
        renderLayers[RenderLayer.Entities].sort((a, b) => {
            return (a.properties.y) - (b.properties.y);
        });

        // Render entities and player with y-sorting
        renderLayers[RenderLayer.Entities].forEach(({entity, properties}) => {
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
                Graphics.drawSpriteEx(x, y, spriteIndex, spriteSheet, flipX, false, 1, ox, oy);
            }
        });

        // Render foreground
        renderLayers[RenderLayer.Foreground].forEach(({entity, properties: {x, y, spriteIndex, spriteSheet, flipX, ox, oy}}) => {
            Graphics.drawSprite(x, y, spriteIndex, spriteSheet, flipX);
        });
    }
}

export const drawSystem = new DrawSystem();