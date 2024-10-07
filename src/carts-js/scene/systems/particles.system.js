class ParticlesSystem {
    constructor() {
        this.emitters = [];
    }

    addEmitter(emitter) {
        this.emitters.push(emitter);
    }

    update(deltaTime) {
        this.emitters.forEach(e => e.update(deltaTime));
    }

    clear() {
        this.emitters = [];
    }
}

export const particlesSystem = new ParticlesSystem();