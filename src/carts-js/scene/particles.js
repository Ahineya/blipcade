import {RenderLayer} from "./systems/draw.system";

function uniqueId() {
    return Math.random().toString(36).substr(2, 9);
}

export class ParticlesEmitter {
    constructor(
        name = "Particles Emitter",
        position = { x: 0, y: 0 },
        emissionRate = 0,
        particleLifetime = 0,
        initialVelocity = { x: 0, y: 0 },
        velocityVariation = 0,
        particleClass = Particle
    ) {
        this.id = uniqueId();
        this.particleClass = particleClass;
        // Let's create more interesting particles emitter:
        const particleEmitterComponent = {
            name,
            id: this.id,
            position,
            emissionRate,
            particleLifetime,
            initialVelocity,
            velocityVariation,
        };

        const particleEmitterEntity = ECS.createEntity();

        ECS.addComponent(particleEmitterEntity, "ParticleEmitter", particleEmitterComponent);
        ECS.addComponent(particleEmitterEntity, "Render", {
            layer: RenderLayer.Entities
        });
    }

    update(deltaTime) {
        // deltaTime is in milliseconds, we need to convert it to seconds
        deltaTime = deltaTime / 1000;

        this.updateEmitter(deltaTime);
        this.updateParticles(deltaTime);
        this.emitParticles(deltaTime);
    }

    updateEmitter(deltaTime) {
        // Let's move the emitter around in a circle
        // ECS.forEachEntity(["ParticleEmitter"], (entity) => {
        //     const emitter = ECS.getComponent(entity, "ParticleEmitter");
        //
        //     const angle = Math.PI * 2 * Date.now() / 5000;
        //     const radius = 20;
        //
        //     emitter.position.x = 60 + Math.cos(angle) * radius;
        //     emitter.position.y = 60 + Math.sin(angle) * radius
        // });

        // No, let's move the emitter with the mouse
        // ECS.forEachEntity(["ParticleEmitter"], (_, emitter) => {
        //     const mouse = Input.getMousePos();
        //     emitter.position.x = mouse.x;
        //     emitter.position.y = mouse.y;
        // });

        // No, even better, let's make the emitter slowly approach the mouse
        // ECS.forEachEntity(["ParticleEmitter"], (entity) => {
        //     const emitter = ECS.getComponent(entity, "ParticleEmitter");
        //     const mouse = Input.getMousePos();
        //     const dx = mouse.x - emitter.position.x;
        //     const dy = mouse.y - emitter.position.y;
        //     const distance = Math.sqrt(dx * dx + dy * dy);
        //     const speed = 100;
        //     const maxDistance = 2;
        //
        //     if (distance > maxDistance) {
        //         emitter.position.x += dx / distance * speed * deltaTime;
        //         emitter.position.y += dy / distance * speed * deltaTime;
        //     }
        // });
    }

    updateParticles(deltaTime) {
        ECS.forEachEntity(["Particle"], (entity, particle) => {
            if (particle.emitterId !== this.id) {
                return;
            }

            this.particleClass.update(particle, deltaTime);

            // Remove dead particles
            if (particle.lifetime <= 0) {
                ECS.destroyEntity(entity);
            }
        });
    }

    emitParticles(deltaTime) {
        ECS.forEachEntity(["ParticleEmitter"], (_, emitter) => {
            if (emitter.id !== this.id) {
                return;
            }

            const particlesToEmit = emitter.emissionRate * deltaTime;
            const wholeParticles = Math.floor(particlesToEmit);
            const fractionalPart = particlesToEmit - wholeParticles;

            // Emit whole particles
            for (let i = 0; i < wholeParticles; i++) {
                this.emitParticle(emitter);
            }

            // Probabilistically emit fractional particle
            if (Math.random() < fractionalPart) {
                this.emitParticle(emitter);
            }
        });
    }

    emitParticle(emitter) {
        new this.particleClass(emitter);
    }

    // Let's draw the particles
    // draw() {
        // ECS.forEachEntity(["Particle"], (_, particle) => {
        //     if (particle.emitterId !== this.id) {
        //         return;
        //     }
        //
        //     // this.particleClass.draw(particle);
        // });

        // Draw the emitter
        // ECS.forEachEntity(["ParticleEmitter"], (entity) => {
        //     const emitter = ECS.getComponent(entity, "ParticleEmitter");
        //     Graphics.drawFilledCircle(emitter.position.x, emitter.position.y, 3, 0xFA);
        // });
    // }
}

export class Particle {
    constructor(emitter) {
        this.emitter = emitter;

        const particleEntity = ECS.createEntity();

        // ECS.addComponent(particleEntity, "Type", {
        //     type: "FireParticle"
        // });

        // Let's make the particles emit in a random direction
        // const angle = Math.random() * Math.PI * 2;
        // const speed = emitter.initialVelocity.y + Math.random() * emitter.velocityVariation;
        // const velocity = {
        //     x: Math.cos(angle) * speed,
        //     y: Math.sin(angle) * speed
        // }

        // Let's make the particles emit in a cone, to the top:
        const angle = (-Math.PI / 2 + Math.random() * Math.PI / 4 - Math.PI / 8);

        const speed = emitter.initialVelocity.y + Math.random() * this.emitter.velocityVariation;
        const velocity = {
            x: Math.cos(angle) * speed,
            y: Math.sin(angle) * speed
        }

        const particle = {
            name: `Particle ${particleEntity}`,
            emitterId: emitter.id,
            position: {
                x: emitter.position.x,
                y: emitter.position.y
            },
            velocity,
            lifetime: emitter.particleLifetime,
            size: 2.9,
            color: 0,
            type: "FireParticle"
        };

        ECS.addComponent(particleEntity, "Particle", particle);
    }

    static update(particle, deltaTime) {
        particle.position.x += particle.velocity.x * deltaTime;
        particle.position.y += particle.velocity.y * deltaTime;

        // Fade out particle

        // Let's determine color based on lifetime
        particle.color = 6 - Math.floor(6 * particle.lifetime / 2);

        // particle.color = Math.min(6, particle.color + 1); // This should be in particle class
        particle.size = Math.max(0, particle.size - deltaTime); // This should be in particle class

        particle.lifetime -= deltaTime;
    }

    static draw(particle) {
        const color = particle.color;
        const size = particle.size;
        const x = particle.position.x;
        const y = particle.position.y;

        // Draw the particle
        Graphics.drawFilledCircle(x, y, size, Particle.gradient[color] || 0xFA);
    }
}

// Let's build the gradient. Palette is 6-8-5 levels RGB. Color index formula is (40×R)+(5×G)+B
// Let's get G 1-5, R 5, B 2

// 5 levels of green
Particle.gradient = [];
for (let i = 0; i < 8; i++) {
    Particle.gradient.push( (40 * 4) + (5 * i) + 1 );
}

export class MiasmaParticle {
    constructor(emitter) {
        this.emitter = emitter;

        const particleEntity = ECS.createEntity();

        // ECS.addComponent(particleEntity, "Type", {
        //     type: "MiasmaParticle"
        // });

        // Let's make the particles emit in a random direction
        // const angle = Math.random() * Math.PI * 2;
        // const speed = emitter.initialVelocity.y + Math.random() * emitter.velocityVariation;
        // const velocity = {
        //     x: Math.cos(angle) * speed,
        //     y: Math.sin(angle) * speed
        // }

        // Let's make the particles emit in a cone, to the top:
        const angle = (-Math.PI / 2 + Math.random() * Math.PI / 4 - Math.PI / 8);

        const speed = this.emitter.initialVelocity.y + Math.random() * this.emitter.velocityVariation;
        const velocity = {
            x: Math.cos(angle) * speed,
            y: Math.sin(angle) * speed
        }

        // Let's make initial particle position random in a circle
        const angle2 = Math.random() * Math.PI * 2;
        const radius = 3;
        const position = {
            x: emitter.position.x + Math.cos(angle2) * radius,
            y: emitter.position.y + Math.sin(angle2) * radius
        }

        const particle = {
            name: `Particle ${particleEntity}`,
            emitterId: emitter.id,
            position,
            velocity,
            // lifetime: emitter.particleLifetime,
            // Let's make it random from 1 to 2 seconds
            lifetime: emitter.particleLifetime * Math.random(),
            size: 2.9,
            color: 0,
            type: "MiasmaParticle"
        };

        ECS.addComponent(particleEntity, "Particle", particle);
    }

    static update(particle, deltaTime) {
        particle.position.x += particle.velocity.x * deltaTime;
        particle.position.y += particle.velocity.y * deltaTime;

        // Fade out particle

        // Let's determine color based on lifetime
        // particle.color = 6 - Math.floor(6 * particle.lifetime / 2);
        // Clamp color to 0-6
        // particle.color = Math.min(6, particle.color);
        // particle.color = Math.max(0, particle.color);

        // Let's make color random (0 - 6)
        // particle.color = Math.floor(Math.random() * 7);
        // No, random color each frame is too much. Let's change it each second
        // particle.color = Math.floor(Date.now() / 1000) % 7;
        // We are near, but let's make it account for lifetime and time both, still having 7 colors
        particle.color = (Math.floor(7 * particle.lifetime / 2) + Math.floor(Date.now() / 1000) % 7) % 7;

        // particle.color = Math.min(6, particle.color + 1); // This should be in particle class
        // particle.size = Math.max(0, particle.size - deltaTime); // This should be in particle class

        particle.lifetime -= deltaTime;
    }

    static draw(particle) {
        const color = particle.color;
        const size = particle.size;
        const x = particle.position.x;
        const y = particle.position.y;

        // Draw the particle
        Graphics.putPixel(x, y, MiasmaParticle.colors[color] || 0xFA);
    }
}

MiasmaParticle.colors = [
    15,
    20,
    25,
    15,
    20,
    15,
    25
];