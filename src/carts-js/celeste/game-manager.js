import {Input} from "./input";
import {state} from "./state";
import {level_index} from "./helpers";
import {drawMain} from "./draw-main";
import {PICO8_COLORS} from "./colors";
import {load_room} from "./load_room";
import {pmusic, psfx} from "./sound";

export const GameState = {
    TITLE: 'title',
    GAME: 'game',
    GAME_OVER: 'game_over'
};

export class GameManager {
    constructor() {
        this.state = GameState.TITLE;

        for (let i = 0; i < 16; i++) {
            state.clouds.push({
                x: Math.random() * 256,
                y: Math.random() * 256,
                spd: 1 + Math.random() * 4,
                w: 32 + Math.random() * 32
            });
        }

        for (let i = 0; i < 25; i++) {
            state.particles.push({
                x: Math.random() * 256,
                y: Math.random() * 256,
                s: 0 + Math.floor(Math.random() * 5) / 4,
                spd: 0.25 + Math.random() * 5,
                off: Math.random(),
                c: PICO8_COLORS[6 + Math.floor(0.5 + Math.random())]
            });
        }

        state.room = { x: 7, y: 3 };
        pmusic(40, 0, 7);
    }

    update() {
        switch(this.state) {
            case GameState.TITLE:
                this.updateTitle();
                this.updateGame();
                break;
            case GameState.GAME:
                this.updateGame();
                break;
            case GameState.GAME_OVER:
                this.updateGameOver();
                break;
        }
    }

    draw() {
        switch(this.state) {
            case GameState.TITLE:
                this.drawTitle();
                break;
            case GameState.GAME:
                this.drawGame();
                break;
            case GameState.GAME_OVER:
                this.drawGameOver();
                break;
        }
    }

    updateTitle() {
        if (Input.isButtonPressed(4) || Input.isButtonPressed(5) && !state.start_game) { // Jump or Dash button
            state.start_game = true;
            state.start_game_flash = 50;
            pmusic(-1);
            psfx(38);
        }

        if (state.start_game) {
            state.start_game_flash--;
            if (state.start_game_flash < -30) {
                this.beginGame();
            }
        }
    }

    drawTitle() {
        drawMain();
        text(58, 60, "X+C", PICO8_COLORS[7]);
        text(42, 80, "MATT THORSON", PICO8_COLORS[7]);
        text(46, 87, "NOEL BERRY", PICO8_COLORS[7]);
        text(34, 100, "BLIPCADE PORT BY", PICO8_COLORS[7]);
        text(47, 107, "@PAVLO_IT", PICO8_COLORS[7]);
    }

    beginGame() {
        this.state = GameState.GAME;
        state.frames = 0;
        state.deaths = 0;
        state.max_djump = 1;
        state.start_game = false;
        state.music_timer = 0;
        pmusic(0, 0, 7);
        load_room(0, 0);
    }

    updateGame() {
        state.frames = (state.frames + 1) % 60;
        if (state.frames === 0 && level_index() < 30) {
            state.seconds = (state.seconds + 1) % 60;
            if (state.seconds === 0) {
                state.minutes += 1;
            }
        }

        if (state.music_timer > 0) {
            state.music_timer -= 1;
            if (state.music_timer <= 0) {
                pmusic(10, 0, 7);
            }
        }

        if (state.sfx_timer > 0) {
            state.sfx_timer -= 1;
        }

        if (state.freeze > 0) {
            state.freeze -= 1;
            return;
        }

        /**
         if shake>0 then
         shake-=1
         camera()
         if shake>0 then
         camera(-2+rnd(5),-2+rnd(5))
         end
         end
         */

        if (state.shake > 0) {
            state.shake -= 1;
            camera(0, 0);
            if (state.shake > 0) {
                camera(-2 + Math.random() * 5, -2 + Math.random() * 5);
            }
        }

        // Update objects
        state.objects.forEach(obj => {
            if (obj.move) obj.move(obj.spd.x, obj.spd.y);
            if (obj.update) obj.update();
        });

        // Handle game restart
        if (state.will_restart && state.delay_restart > 0) {
            state.delay_restart -= 1;
            if (state.delay_restart <= 0) {
                state.will_restart = false;
                load_room(state.room.x, state.room.y);
            }
        }
    }

    drawGame() {
        drawMain();
    }

    updateGameOver() {
        if (Input.isButtonPressed(4) || Input.isButtonPressed(5)) { // Jump or Dash button
            this.beginGame();
        }
    }

    drawGameOver() {
        fillScreen(0);
        text(44, 40, "GAME OVER", 7);
        text(48, 60, "DEATHS: " + state.deaths, 5);
        text(12, 80, "PRESS JUMP TO RESTART", 5);
    }
}

export const gameManager = new GameManager();