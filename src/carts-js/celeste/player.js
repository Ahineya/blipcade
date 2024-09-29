import {state} from "./state";
import {Input} from "./input";
import {init_object} from "./init_object";
import {Platform} from "./platform";
import {FallFloor} from "./fallfloor";
import {FakeWall} from "./fakewall";
import {destroy_object, ice_at, level_index, restart_room, sign, solid_at, tile_at} from "./helpers";
import {next_room} from "./next_room";
import {Smoke} from "./smoke";
import {hexToColor, PICO8_COLORS} from "./colors";
import {psfx} from "./sound";

function rnd(number) {
    const result = Math.floor(Math.random() * number);
    return result;
}

function clamp(x, number, number2) {
    return Math.min(Math.max(x, number), number2);
}

export class Player {
    solids = true;

    constructor(x, y) {
        this.x = x;
        this.y = y;
        this.spd = {x: 0, y: 0};
        this.rem = {x: 0, y: 0};
        this.hitbox = {x: 1, y: 3, w: 6, h: 5};
        this.spr = 1;
        this.flip = {x: false, y: false};
        this.p_jump = false;
        this.p_dash = false;
        this.grace = 0;
        this.jbuffer = 0;
        this.djump = state.max_djump;
        this.dash_time = 0;
        this.dash_effect_time = 0;
        this.dash_target = {x: 0, y: 0};
        this.dash_accel = {x: 0, y: 0};
        this.spr_off = 0;
        this.was_on_ground = false;
        this.init();

        this.debugInfo = {
            lastUpdateTime: Date.now(),
            frameCount: 0,
            totalDistance: 0
        };
    }

    init() {
        this.create_hair();
    }

    create_hair() {
        this.hair = [];
        for (let i = 0; i < 5; i++) {
            this.hair.push({x: this.x, y: this.y, size: Math.max(1, Math.min(2, 3 - i))});
        }
    }

    update() {
        if (state.pause_player) return;

        const now = Date.now();
        const dt = (now - this.debugInfo.lastUpdateTime) / 1000; // in seconds
        this.debugInfo.lastUpdateTime = now;
        this.debugInfo.frameCount++;

        let input = (Input.isButtonPressed(1) ? 1 : 0) - (Input.isButtonPressed(0) ? 1 : 0);

        // spikes collide
        if (this.spikes_at(this.x + this.hitbox.x, this.y + this.hitbox.y, this.hitbox.w, this.hitbox.h, this.spd.x, this.spd.y)) {
            this.kill();
        }

        // bottom death
        if (this.y > 128) {
            this.kill();
        }

        let on_ground = this.is_solid(0, 1);
        let on_ice = this.is_ice(0, 1);

        // smoke particles
        if (on_ground && !this.was_on_ground) {
            init_object(Smoke, this.x, this.y + 4);
        }

        let jump = Input.isButtonPressed(4) && !this.p_jump;
        this.p_jump = Input.isButtonPressed(4);
        if (jump) {
            this.jbuffer = 4;
        } else if (this.jbuffer > 0) {
            this.jbuffer--;
        }

        let dash = Input.isButtonPressed(5) && !this.p_dash;
        this.p_dash = Input.isButtonPressed(5);

        if (on_ground) {
            this.grace = 6;
            if (this.djump < state.max_djump) {
                psfx(54);
                this.djump = state.max_djump;
            }
        } else if (this.grace > 0) {
            this.grace--;
        }

        this.dash_effect_time--;
        if (this.dash_time > 0) {
            init_object(Smoke, this.x, this.y);
            this.dash_time--;
            this.spd.x = this.appr(this.spd.x, this.dash_target.x, this.dash_accel.x);
            this.spd.y = this.appr(this.spd.y, this.dash_target.y, this.dash_accel.y);
        } else {
            // move
            let maxrun = 1;
            let accel = 0.6;
            let deccel = 0.15;
            // gravity
            let maxfall = 2;
            let gravity = 0.21;

            if (Math.abs(this.spd.y) <= 0.15) {
                gravity *= 0.5;
            }

            if (!on_ground) {
                accel = 0.4;
            } else if (on_ice) {
                accel = 0.05;
                if (input === (this.flip.x ? -1 : 1)) {
                    accel = 0.05;
                }
            }

            if (Math.abs(this.spd.x) > maxrun) {
                this.spd.x = this.appr(this.spd.x, sign(this.spd.x) * maxrun, deccel);
            } else {
                this.spd.x = this.appr(this.spd.x, input * maxrun, accel);
            }

            if (this.spd.x !== 0) {
                this.flip.x = (this.spd.x < 0);
            }

            // wall slide
            if (input !== 0 && this.is_solid(input, 0) && !this.is_ice(input, 0)) {
                maxfall = 0.4;
                if (rnd(10) < 2) {
                    init_object(Smoke, this.x + input * 6, this.y);
                }
            }

            if (!on_ground) {
                this.spd.y = this.appr(this.spd.y, maxfall, gravity);
            }

            // jump
            if (this.jbuffer > 0) {
                if (this.grace > 0) {
                    // normal jump
                    psfx(1);
                    this.jbuffer = 0;
                    this.grace = 0;
                    this.spd.y = -2;
                    init_object(Smoke, this.x, this.y + 4);
                } else {
                    // wall jump
                    let wall_dir = (this.is_solid(-3, 0) && -1) || (this.is_solid(3, 0) && 1) || 0;
                    if (wall_dir !== 0) {
                        psfx(2);
                        this.jbuffer = 0;
                        this.spd.y = -2;
                        this.spd.x = -wall_dir * (maxrun + 1);
                        if (!this.is_ice(wall_dir * 3, 0)) {
                            init_object(Smoke, this.x + wall_dir * 6, this.y);
                        }
                    }
                }
            }

            // dash
            let d_full = 5;
            let d_half = d_full * 0.70710678118;

            if (this.djump > 0 && dash) {
                init_object(Smoke, this.x, this.y);
                this.djump--;
                this.dash_time = 4;
                state.has_dashed = true;
                this.dash_effect_time = 10;
                let v_input = (Input.isButtonPressed(2) && -1) || (Input.isButtonPressed(3) && 1) || 0;
                if (input !== 0) {
                    if (v_input !== 0) {
                        this.spd.x = input * d_half;
                        this.spd.y = v_input * d_half;
                    } else {
                        this.spd.x = input * d_full;
                        this.spd.y = 0;
                    }
                } else if (v_input !== 0) {
                    this.spd.x = 0;
                    this.spd.y = v_input * d_full;
                } else {
                    this.spd.x = (this.flip.x ? -1 : 1);
                    this.spd.y = 0;
                }

                psfx(3);
                state.freeze = 2;
                state.shake = 6;
                this.dash_target.x = 2 * sign(this.spd.x);
                this.dash_target.y = 2 * sign(this.spd.y);
                this.dash_accel.x = 1.5;
                this.dash_accel.y = 1.5;

                if (this.spd.y < 0) {
                    this.dash_target.y *= 0.75;
                }

                if (this.spd.y !== 0) {
                    this.dash_accel.x *= 0.70710678118;
                }
                if (this.spd.x !== 0) {
                    this.dash_accel.y *= 0.70710678118;
                }
            } else if (dash && this.djump <= 0) {
                psfx(9);
                init_object(Smoke, this.x, this.y);
            }
        }

        // animation
        this.spr_off += 0.25;
        if (!on_ground) {
            if (this.is_solid(input, 0)) {
                this.spr = 5;
            } else {
                this.spr = 3;
            }
        } else if (Input.isButtonPressed(3)) {
            this.spr = 6;
        } else if (Input.isButtonPressed(2)) {
            this.spr = 7;
        } else if (this.spd.x === 0 || (!Input.isButtonPressed(0) && !Input.isButtonPressed(1))) {
            this.spr = 1;
        } else {
            this.spr = 1 + (this.spr_off % 4);
        }

        // this.x += this.spd.x;
        // this.y += this.spd.y;

        // Clamp in screen
        if (this.x < -1 || this.x > 121) {
            this.x = clamp(this.x, -1, 121);
            this.spd.x = 0;
        }

        // next level
        if (this.y < -4 && level_index() < 30) {
            next_room();
        }

        // was on the ground
        this.was_on_ground = on_ground;

        // This is the fix for the movement for this port. I have no idea why the original code is not working the same way
        const MOVE_SCALE = 0.6;
        this.rem.x += this.spd.x * MOVE_SCALE;
        this.rem.y += this.spd.y * MOVE_SCALE;
    }

    draw() {
        // clamp in screen
        if (this.x < -1 || this.x > 121) {
            this.x = clamp(this.x, -1, 121);
            this.spd.x = 0;
        }

        this.set_hair_color(this.djump);
        this.draw_hair();
        drawSprite(this.spr, this.x, this.y, this.flip.x, this.flip.y); // TODO: Implement drawSprite

        this.unset_hair_color();

        // draw hitbox collider collision box
        // drawRect(this.x + this.hitbox.x, this.y + this.hitbox.y, this.x + this.hitbox.w, this.y + this.hitbox.h, 8);
    }

    move(ox, oy) {
        let amount;
        // [x] get move amount
        this.rem.x += ox;
        amount = Math.floor(this.rem.x + 0.5);
        this.rem.x -= amount;
        this.move_x(amount, 0);

        // [y] get move amount
        this.rem.y += oy;
        amount = Math.floor(this.rem.y + 0.5);
        this.rem.y -= amount;
        this.move_y(amount);
    }

    move_x(amount, start) {
        if (this.solids) {
            let step = Math.sign(amount);
            for (let i = start; i !== Math.abs(amount); i++) {
                if (!this.is_solid(step, 0)) {
                    this.x += step;
                } else {
                    this.spd.x = 0;
                    this.rem.x = 0;
                    break;
                }
            }
        } else {
            this.x += amount;
        }
    }

    move_y(amount) {
        if (this.solids) {
            let step = Math.sign(amount);
            for (let i = 0; i !== Math.abs(amount); i++) {
                if (!this.is_solid(0, step)) {
                    this.y += step;
                } else {
                    this.spd.y = 0;
                    this.rem.y = 0;
                    break;
                }
            }
        } else {
            this.y += amount;
        }
    }

    set_hair_color(djump) {
        let color = PICO8_COLORS[8];
        if (djump === 1) {
            color = PICO8_COLORS[8];
        } else if (djump === 2) {
            color = (Math.floor(state.frames / 3) % 2 === 0) ? PICO8_COLORS[7] : PICO8_COLORS[11];
        } else {
            color = PICO8_COLORS[12];
        }
        changeColors(PICO8_COLORS[8], color);
    }

    unset_hair_color() {
        resetColors(); // TODO: May need to fix changeColors, so the second arg will be not a virtual color
    }

    draw_hair() {
        let last = {x: this.x + 4 - (this.flip.x ? 2 : 0), y: this.y + (Input.isButtonPressed(3) ? 4 : 3)};
        for (let h of this.hair) {
            h.x += (last.x - h.x) / 1.5;
            h.y += (last.y + 0.5 - h.y) / 1.5;
            drawFilledCircle(h.x, h.y, h.size, PICO8_COLORS[8]);
            last = h;
        }
    }

    is_solid(ox, oy) {
        if (oy > 0 && !this.check(Platform, ox, 0) && this.check(Platform, ox, oy)) {
            log("Platform collision");
            return true;
        }

        const c = solid_at(this.x + this.hitbox.x + ox, this.y + this.hitbox.y + oy, this.hitbox.w, this.hitbox.h)
            || this.check(FallFloor, ox, oy)
            || this.check(FakeWall, ox, oy);

        return c;
    }

    is_ice(ox, oy) {
        return ice_at(this.x + this.hitbox.x + ox, this.y + this.hitbox.y + oy, this.hitbox.w, this.hitbox.h);
    }

    check(type, ox, oy) {
        for (let other of state.objects) {
            if (other instanceof FakeWall) {
                if (other.collideable &&
                    other.x + other.hitbox.x + other.hitbox.w > this.x + this.hitbox.x + ox &&
                    other.y + other.hitbox.y + other.hitbox.h > this.y + this.hitbox.y + oy &&
                    other.x + other.hitbox.x < this.x + this.hitbox.x + this.hitbox.w + ox &&
                    other.y + other.hitbox.y < this.y + this.hitbox.y + this.hitbox.h + oy) {
                }
            }

            if (other instanceof type && other !== this && other.collideable &&
                other.x + other.hitbox.x + other.hitbox.w > this.x + this.hitbox.x + ox &&
                other.y + other.hitbox.y + other.hitbox.h > this.y + this.hitbox.y + oy &&
                other.x + other.hitbox.x < this.x + this.hitbox.x + this.hitbox.w + ox &&
                other.y + other.hitbox.y < this.y + this.hitbox.y + this.hitbox.h + oy) {
                return other;
            }
        }
        return null;
    }

    kill() {
        state.sfx_timer = 12;
        psfx(0);
        state.deaths++;
        state.shake = 10;
        destroy_object(this); // Implement this to remove the player from the game objects
        state.dead_particles = [];
        for (let dir = 0; dir < 8; dir++) {
            const angle = (dir / 8) * 2 * Math.PI; // Convert to radians
            state.dead_particles.push({
                x: this.x + 4,
                y: this.y + 4,
                t: 10,
                spd: {
                    x: Math.sin(angle) * 3,
                    y: Math.cos(angle) * 3
                }
            });
        }
        restart_room();
    }

    appr(val, target, amount) {
        return val > target ?
            Math.max(val - amount, target) :
            Math.min(val + amount, target);
    }

    spikes_at(x, y, w, h, xspd, yspd) {
        for (let i = Math.max(0, Math.floor(x / 8)); i <= Math.min(15, Math.floor((x + w - 1) / 8)); i++) {
            for (let j = Math.max(0, Math.floor(y / 8)); j <= Math.min(15, Math.floor((y + h - 1) / 8)); j++) {
                const tile = tile_at(i, j);
                if (tile === 17 && ((y + h - 1) % 8 >= 6 || y + h === j * 8 + 8) && yspd >= 0) {
                    return true;
                } else if (tile === 27 && y % 8 <= 2 && yspd <= 0) {
                    return true;
                } else if (tile === 43 && x % 8 <= 2 && xspd <= 0) {
                    return true;
                } else if (tile === 59 && ((x + w - 1) % 8 >= 6 || x + w === i * 8 + 8) && xspd >= 0) {
                    return true;
                }
            }
        }
        return false;
    }
}