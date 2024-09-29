import {APP_STATE, state} from "./state/state";
import {Loading} from "./loading/loading";
import {sc} from "../framework/scene";
import {w} from "../framework/widget";
import {B} from "../framework/b";
import {ParticleManager} from "./particle-manager";

global.init = () => {
    setTransparentColor(255);

    const mainScene = sc(0, 0, [
        sc(4, 4, [
            w(40, 40, () => text(40, 0, 'Hello, world!')),
        ])
    ]);

    state.SPRITES = {
        CURSOR: 0
    }

    state.particleManager = new ParticleManager();
    const loading = new Loading();
    state.objects.push(loading);

    B.setScene(mainScene);

    global.once = true;

    playSound(0, 0);
}

global.update = () => {
    // Mouse.mouseUpdate(getMouseState());

    const currentTime = Date.now();
    const elapsed = currentTime - state.lastUpdateTime;

    if (elapsed >= state.FRAME_TIME) {
        if (state.appState === APP_STATE.LOADING) {
            // Update here
            state.frames = (state.frames + 1) % 30;
            if (state.frames === 0) {
                state.seconds = (state.seconds + 1) % 60;
            }

            state.objects.forEach(obj => {
                obj.update?.();
            });

            state.lastUpdateTime = currentTime - (elapsed % state.FRAME_TIME);
        } else if (state.appState === APP_STATE.OS) {
            loadCart('@os');
        }
    }

    if (state.appState === APP_STATE.LOADED || isKeyPressed(32) || isKeyPressed(16)) {
        let cartPath = global.CART_PATH;

        if (!cartPath) {
            state.appState = APP_STATE.OS;
            return
        }

        loadCart(cartPath);
    }
}

global.draw = () => {
    const currentTime = Date.now();
    const elapsed = currentTime - state.lastDrawTime;
    if (elapsed >= state.FRAME_TIME) {
        state.lastDrawTime = currentTime - (elapsed % state.FRAME_TIME);
        fillScreen(0);
    //
        if (state.appState === APP_STATE.LOADING) {
            state.objects.forEach(obj => {
                obj.draw?.();
            });

            state.particleManager.particles.filter(p => p.layer === 0).forEach(particle => {
                particle.update?.();
                particle.draw();
            });

            state.objects.forEach(obj => {
                obj.draw?.();
            });

            state.particleManager.particles.filter(p => p.layer === 1).forEach(particle => {
                particle.update?.();
                particle.draw();
            });
        } else if (state.appState === APP_STATE.DEVELOPMENT) {
            B.getScene().render();
        }
    }
}
