export class ParticlesSystem {
    constructor() {

        // Let's create more interesting particles emitter:
        const particleEmitterComponent = {
            name: "Particle Emitter",
            position: { x: 60, y: 60 },
            emissionRate: 2000,
            particleLifetime: 0.5,
            initialVelocity: { x: 0, y: 20 },
            velocityVariation: 50
        };

        const particleEmitterEntity = ECS.createEntity();

        ECS.addComponent(particleEmitterEntity, "ParticleEmitter", particleEmitterComponent);
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
        ECS.forEachEntity(["ParticleEmitter"], (entity) => {
            const emitter = ECS.getComponent(entity, "ParticleEmitter");
            const mouse = Input.getMousePos();
            emitter.position.x = mouse.x;
            emitter.position.y = mouse.y;
        });

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
        ECS.forEachEntity(["Particle"], (entity) => {
            const particle = ECS.getComponent(entity, "Particle");

            particle.position.x += particle.velocity.x * deltaTime;
            particle.position.y += particle.velocity.y * deltaTime;
            particle.lifetime -= deltaTime;

            // Fade out particle
            particle.color = Math.max(0, particle.color - 1);

            particle.size = Math.min(5, particle.size + 0.01);

            // Remove dead particles
            if (particle.lifetime <= 0) {
                ECS.destroyEntity(entity);
            }
        });
    }

    emitParticles(deltaTime) {
        ECS.forEachEntity(["ParticleEmitter"], (entity) => {
            const emitter = ECS.getComponent(entity, "ParticleEmitter");

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
        const particleEntity = ECS.createEntity();

        // Let's make the particles emit in a random direction
        const angle = Math.random() * Math.PI * 2;
        const speed = emitter.initialVelocity.y + Math.random() * emitter.velocityVariation;
        const velocity = {
            x: Math.cos(angle) * speed,
            y: Math.sin(angle) * speed
        }

        const particle = {
            name: `Particle ${particleEntity}`,
            position: {
                x: emitter.position.x,
                y: emitter.position.y
            },
            velocity,
            lifetime: emitter.particleLifetime,
            size: 2,
            color: 0xFF
        };

        ECS.addComponent(particleEntity, "Particle", particle);
    }

    // Let's draw the particles
    draw() {
        ECS.forEachEntity(["Particle"], (entity) => {
            const particle = ECS.getComponent(entity, "Particle");
            const color = particle.color;
            const size = particle.size;
            const x = particle.position.x;
            const y = particle.position.y;

            // Draw the particle
            Graphics.drawFilledCircle(x, y, size, color);
        });

        // Draw the emitter
        // ECS.forEachEntity(["ParticleEmitter"], (entity) => {
        //     const emitter = ECS.getComponent(entity, "ParticleEmitter");
        //     Graphics.drawFilledCircle(emitter.position.x, emitter.position.y, 3, 0xFA);
        // });
    }
}
