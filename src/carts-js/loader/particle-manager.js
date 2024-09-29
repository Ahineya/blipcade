export class ParticleManager {
    particles = [];

    constructor() {
    }

    draw() {
        this.particles.forEach(particle => {
            particle?.update();
            particle.draw();
        });
    }
}
