import {state} from "./state";
import {is_title, level_index} from "./helpers";
import {Platform} from "./platform";
import {BigChest} from "./big-chest";
import {PICO8_COLORS} from "./colors";
import {Player} from "./player";

function resetPalette() {
    resetColors();
}

function setPalette(index, color) {
    changeColors(PICO8_COLORS[index], PICO8_COLORS[color]);
}

function draw_object(obj) {
    if (obj.draw !== undefined) {
        obj.draw(obj);
    } else if (obj.spr > 0) {
        drawSprite(obj.spr, obj.x, obj.y, obj.flip.x, obj.flip.y); // TODO: Implement
    }
}

export function drawMain() {
    if (state.freeze > 0) return;

    // Reset all palette values
    resetPalette();

    // Start game flash
    if (state.start_game) {
        let c = 10;
        if (state.start_game_flash > 10) {
            if (state.frames % 10 < 5) {
                c = 7;
            }
        } else if (state.start_game_flash > 5) {
            c = 2;
        } else if (state.start_game_flash > 0) {
            c = 1;
        } else {
            c = 0;
        }

        if (c < 10) {
            setPalette(6, c);
            setPalette(12, c);
            setPalette(13, c);
            setPalette(5, c);
            setPalette(1, c);
            setPalette(7, c);
        }
    }
    // Clear screen
    let bg_col = 0;

    if (state.flash_bg) {
        bg_col = PICO8_COLORS[Math.floor(state.frames / 5)] || 0;
    } else if (state.new_bg !== undefined) {
        bg_col = PICO8_COLORS[2];
    }

    fillScreen(bg_col);
    //
    // // Draw clouds
    if (!is_title()) {
        state.clouds.forEach(function(c) {
            c.x += c.spd;
            drawFilledRectangle(c.x, c.y, c.x + c.w, c.y + 4 + (1 - c.w / 64) * 12, state.new_bg !== undefined ? PICO8_COLORS[14] : PICO8_COLORS[1]);
            if (c.x > 256) {
                c.x = -c.w;
                c.y = Math.random() * (256 - 8);
            }
        });
    }
    //
    // // Draw bg terrain
    drawMap(0, 0, 0, state.room.x * 16, state.room.y * 16, 16, 16, 4);
    //
    // // Draw platforms/big chest
    state.objects.forEach(function(o) {
        // if (o.type === platform || o.type === big_chest) {
        if (o instanceof Platform || o instanceof BigChest) {
            draw_object(o);
        }
    });
    //
    // // Draw terrain
    let off = is_title() ? -4 : 0;
    // drawMap(state.room.x * 16, state.room.y * 16, off, 0, 16, 16, 2);
    drawMap(off, 0, 0, state.room.x * 16, state.room.y * 16, 16, 16, 2);
    //
    // // Draw objects
    state.objects.forEach(function(o) {
        if (!(o instanceof Platform) && !(o instanceof BigChest)) {
            draw_object(o);
        }
    });
    //
    // // Draw fg terrain
    // // drawMap(state.room.x * 16, state.room.y * 16, 0, 0, 16, 16, 8);
    drawMap(0, 0, 0, state.room.x * 16, state.room.y * 16, 16, 16, 8);
    //
    // // Draw particles
    state.particles.forEach((p) => {
        p.x += p.spd;
        p.y += Math.sin(p.off);
        p.off += Math.min(0.05, p.spd / 32);
        drawFilledRectangle(p.x, p.y, p.x + p.s, p.y + p.s, p.c);
        if (p.x > 256 + 4) {
            p.x = -4;
            p.y = Math.random() * 256;
        }
    });
    //
    // // Draw dead particles
    state.dead_particles.forEach((p) => {
        p.x += p.spd.x;
        p.y += p.spd.y;
        p.t -= 1;
        if (p.t <= 0) {
            state.dead_particles = state.dead_particles.filter(dp => dp !== p);
        }
        drawFilledRectangle(p.x - p.t / 5, p.y - p.t / 5, p.x + p.t / 5, p.y + p.t / 5, PICO8_COLORS[14 + p.t % 2]);
    });

    drawFilledRectangle(-5, -5, -1, 133, 0);
    drawFilledRectangle(-5, -5, 133, -1, 0);
    drawFilledRectangle(-5, 256, 133, 133, 0);
    drawFilledRectangle(256, -5, 133, 133, 0);

    if (level_index() === 30) {
        let p = state.objects.find(obj => obj instanceof Player);
        if (p !== undefined) {
            let diff = Math.min(24, 40 - Math.abs(p.x + 4 - 64));
            drawFilledRectangle(0, 0, diff, 256, 0);
            drawFilledRectangle(256 - diff, 0, 256, 256, 0);
        }
    }
}