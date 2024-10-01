import {Particle} from "../../graphics-lib/particle";
import {LOADING_STAGE} from "./loading";
import {B} from "../../framework/b";
import {hexToColor} from "../../framework/colors";
import {state} from "../state/state";
import {lerp} from "../../graphics-lib/lerp";
import {approach2D, approachExp} from "../../graphics-lib/movement";

const screenToSpeed = x => lerp(x, 256, 250, 7, 10);

export class UpRotatingParticle extends Particle {
    constructor(x, y, color, size, speed, angle, stage, towards, SCREEN_FACTOR) {
        super(x, y, color);
        this.speed = speed;
        this.angle = angle;
        this.stage = stage;
        this.prevStage = stage;
        this.size = size;
        this.towards = towards;
        this.layer = 0;
        this.SCREEN_FACTOR = SCREEN_FACTOR;
    }

    updateInitial() {
        this.angle += 0.1;
        this.x += Math.cos(this.angle) * this.speed;
        this.y += Math.sin(this.angle) * this.speed;
        this.color = (this.color + 1) % 240;
    }

    updateFlyIn() {
        if (this.prevStage !== this.stage) {
            this.prevStage = this.stage;
            this.speed = 0;
        }

        const { x: targetX, y: targetY } = this.towards;
        const newPosition = approach2D(this.x, this.y, targetX, targetY, this.speed);
        this.x = newPosition.x;
        this.y = newPosition.y;
        this.speed = 3 * approachExp(this.speed, screenToSpeed(this.SCREEN_FACTOR), 1);

        if (this.x === targetX && this.y === targetY) {
            this.color = hexToColor('#ffffff');
        }
    }

    updateFlyOut() {
        this.angle += 0.1;
        const xMove = Math.cos(this.angle) * this.speed;
        this.layer = +(xMove > 0);
        this.x += xMove;
        this.y += Math.sin(this.angle) * this.speed;

        if (this.isOutOfBounds()) {
            this.removeFromParticleManager();
        }
    }

    isOutOfBounds() {
        return this.x < 0 || this.x > B.SCREEN_SIZE.width ||
            this.y < 0 || this.y > B.SCREEN_SIZE.height;
    }

    removeFromParticleManager() {
        state.particleManager.particles = state.particleManager.particles.filter(p => p !== this);
    }

    draw() {
        switch (this.stage) {
            case LOADING_STAGE.INITIAL:
                this.updateInitial();
                break;
            case LOADING_STAGE.FLY_IN:
                this.updateFlyIn();
                break;
            case LOADING_STAGE.FLY_OUT:
                this.updateFlyOut();
                break;
        }

        drawFilledCircle(this.x, this.y, this.size, this.color);
    }
}
