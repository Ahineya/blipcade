import {UpRotatingParticle} from "./particles";
import {B} from "../../framework/b";
import {Text} from "../../graphics-lib/text";
import {APP_STATE, state} from "../state/state";
import {GlitchAnimator} from "./glitch-animator";
import {approach} from "../../graphics-lib/movement";

export const LOADING_STAGE = {
    INITIAL: 'initial',
    FLY_IN: 'fly-in',
    FLY_OUT: 'fly-out',
    GLITCH: 'glitch'
}

class LoadingState {
    constructor(loading) {
        this.loading = loading;
    }

    enter() {}
    update() {}
    draw() {}
}

class InitialState extends LoadingState {
    enter() {
        this.loading.initializeParticles();
    }

    update() {
        if (state.seconds === 2) {
            this.loading.changeState(new FlyInState(this.loading));
        }
    }
}

class FlyInState extends LoadingState {
    enter() {
        this.loading.updateParticlesStage(LOADING_STAGE.FLY_IN);
        this.loading.countup = 1;
    }

    update() {
        this.loading.color = approach(this.loading.color, 254, 1);
        if (state.seconds === 3) {
            this.loading.changeState(new GlitchState(this.loading));
        }
    }

    draw() {
        text(this.loading.textPos.x, this.loading.textPos.y, this.loading.text, this.loading.color);
    }
}

class GlitchState extends LoadingState {
    enter() {
        this.loading.updateParticlesStage(LOADING_STAGE.FLY_OUT);
    }

    update() {
        this.loading.letterOffsets = this.loading.glitchAnimator.update();
    }

    draw() {
        if (!this.loading.glitchAnimator.isSettled()) {
            this.drawGlitchedText();
        } else {
            this.drawGlitchedText();
            this.loading.exit();
        }
    }

    drawGlitchedText() {
        for (let i = 0; i < this.loading.text.length; i++) {
            const char = this.loading.text.charAt(i);
            const charWidth = Text.width(char);
            const x = this.loading.textPos.x + i * charWidth;
            const y = this.loading.textPos.y + this.loading.letterOffsets[i];
            text(x, y, char, this.loading.color);
        }
    }
}

export class Loading {
    static SCREEN_FACTOR = Math.sqrt(B.SCREEN_SIZE.width * B.SCREEN_SIZE.height);
    static PARTICLES_COUNT = Math.floor(Loading.SCREEN_FACTOR / 3);

    constructor() {
        this.text = "Blipcade";
        this.textPos = Text.screenCenter(this.text);
        this.textHeight = Text.height(this.text);
        this.color = 240;
        this.letterOffsets = new Array(this.text.length).fill(0);
        this.glitchAnimator = new GlitchAnimator(this.text.length);

        this.state = new InitialState(this);
        this.state.enter();
    }

    exit() {
        state.appState = APP_STATE.LOADED;
    }

    initializeParticles() {
        for (let i = 0; i < Loading.PARTICLES_COUNT; i++) {
            const particle = this.createParticle();
            state.particleManager.particles.push(particle);
        }
    }

    createParticle() {
        const x = Math.random() * B.SCREEN_SIZE.width;
        const y = Math.random() * B.SCREEN_SIZE.height;
        const color = Math.random() * 240 + 1;
        const speed = Math.random() * 2 + 1;
        const angle = Math.random() * Math.PI * 2;
        const size = Math.random() * 3;

        return new UpRotatingParticle(x, y, color, size, speed, angle, LOADING_STAGE.INITIAL, {
            x: this.textPos.x - 5,
            y: this.textPos.y + this.textHeight / 2 - size / 2 + 1
        }, Loading.SCREEN_FACTOR);
    }

    changeState(newState) {
        this.state = newState;
        this.state.enter();
    }

    updateParticlesStage(newStage) {
        state.particleManager.particles.forEach(particle => {
            particle.stage = newStage;
        });
    }

    update() {
        this.state.update();
    }

    draw() {
        this.state.draw();
    }
}
