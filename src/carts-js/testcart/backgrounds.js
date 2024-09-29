import {PICO8_COLORS} from "../framework/colors";

const gridcolor = 12;

export class Background {
    constructor() {
        this.t = 0;
        this.list = [ "starfield","loopy", "grid", "balls", "flurry", "ballspiral", "flight", "circles", "rainbow", "spiral", "matrix"];
        this.rand = [];
        this.rand2 = [];
        this.current = null;
        this.currentName = null;

        // Star field properties
        this.star_sx = 64;
        this.star_sy = 64;
        this.star_cutoff = 12;
        this.stars = [];
        this.spots = [];

        this.backgroundTypes = {
            loopy: {
                color: PICO8_COLORS[0],
                mfg: PICO8_COLORS[7],
                mbg: PICO8_COLORS[0],
                hfg: PICO8_COLORS[15],
                pbg: PICO8_COLORS[1],
                load: () => {
                    this.rand = Array.from({length: 128*128}, () => Math.random() * 16);
                },
                draw: () => {
                    this.rand.forEach((p, i) => {
                        const x = i % 128;
                        const y = Math.floor(i / 128);
                        Graphics.putPixel(x, y, PICO8_COLORS[Math.floor(Math.random() * 17)]);
                    });
                }
            },
            flurry: {
                color: PICO8_COLORS[0],
                mfg: PICO8_COLORS[7],
                mbg: PICO8_COLORS[0],
                hfg: PICO8_COLORS[15],
                pbg: PICO8_COLORS[1],
                load: () => {
                    this.rand = [];
                    this.rand2 = [];
                    for (let n = 0; n < 256; n++) {
                        this.rand.push(Math.random() * 128);
                        this.rand2.push(Math.random() * 128);
                    }
                },
                draw: () => {
                    const p = [1, 1, 13, 13, 2, 14, 6];
                    for (let i = 0; i < 256; i++) {
                        const z = Math.floor(i / 40) + 1;
                        Graphics.drawFilledCircle(
                            (this.rand[i] - this.t * z * z) % 140 - 8,
                            this.rand2[i],
                            z,
                            PICO8_COLORS[p[z]] || 0xfe
                        );
                    }
                    this.t += 0.05; // Adjust this value to control the speed
                }
            },

            balls: {
                color: PICO8_COLORS[0],
                mfg: PICO8_COLORS[7],
                mbg: PICO8_COLORS[2],
                hfg: PICO8_COLORS[1],
                pbg: PICO8_COLORS[8],
                load: () => {
                    for (let n = 0; n < 500; n++) {
                        this.rand.push(Math.random() * 9);
                    }
                },
                draw: () => {
                    Graphics.drawFilledCircle(64, 64, 64, PICO8_COLORS[7]);
                    this.t += 0.01;
                }
            },
            rainbow: {
                color: PICO8_COLORS[2],
                mfg: PICO8_COLORS[1],
                mbg: PICO8_COLORS[7],
                hfg: PICO8_COLORS[2],
                pbg: PICO8_COLORS[2],
                draw: () => {
                    const m = 12;
                    this.t += 0.05;
                    for (let n = 0; n < m * m; n++) {
                        const x = n % m;
                        const y = Math.floor(n / m);
                        Graphics.drawFilledCircle(
                            x * 12,
                            y * 12,
                            12 + 4 * Math.cos(this.t + y / 5) + 4 * Math.sin(this.t + x / 7),
                            PICO8_COLORS[8 + (x + y) % 8] || 0xfe
                        );
                    }
                }
            },
            spiral: {
                color: PICO8_COLORS[1],
                mfg: PICO8_COLORS[7],
                mbg: PICO8_COLORS[5],
                hfg: PICO8_COLORS[6],
                pbg: PICO8_COLORS[0],
                draw: () => {
                    const s = 64;
                    this.t += 0.0001;
                    let x = s, y = s;
                    for (let i = 1; i <= 350; i++) {
                        const j = i * this.t;
                        const u = x, v = y;
                        x = x + j * Math.sin(j);
                        y = y + j * Math.cos(j);
                        Graphics.drawLine(u, v, x, y, PICO8_COLORS[7 + Math.floor(i / 60)] || 0xfe);
                    }
                }
            },
            circles: {
                color: PICO8_COLORS[2],
                mfg: PICO8_COLORS[7],
                mbg: PICO8_COLORS[1],
                hfg: PICO8_COLORS[15],
                pbg: PICO8_COLORS[0],
                draw: () => {
                    this.t += 0.01;
                    for (let k = 0; k <= 16; k++) {
                        for (let n = 1; n <= 9; n++) {
                            const h = k / 16 + this.t;
                            Graphics.drawCircle(
                                64 + Math.cos(h + this.t / 3) * n * 8,
                                64 + Math.sin(h) * (n * n + Math.cos(this.t) * 16),
                                n,
                                PICO8_COLORS[11 - Math.floor(n / 3)] || 0xfe
                            );
                        }
                    }
                }
            },
            ballspiral: {
                color: PICO8_COLORS[9],
                mfg: PICO8_COLORS[0],
                mbg: PICO8_COLORS[7],
                hfg: PICO8_COLORS[4],
                pbg: PICO8_COLORS[4],
                draw: () => {
                    let x = 64, y = 64, r = 1, a = 0;
                    for (let i = 0; i <= 150; i++) {
                        const c = i % 2 === 0 ? 14 : 7;
                        Graphics.drawFilledCircle(x, y, r / 2, PICO8_COLORS[6 + i % 3] || 0xfe);
                        x += Math.cos(a) * r;
                        y += Math.sin(a) * r;
                        r += 1 / 4;
                        a += this.t / 5;
                    }
                    this.t += 0.002;
                }
            },
            grid: {
                color: PICO8_COLORS[0],
                mfg: PICO8_COLORS[7],
                mbg: PICO8_COLORS[2],
                hfg: PICO8_COLORS[13],
                pbg: PICO8_COLORS[2],
                draw: () => {
                    const w = 127;
                    const n = 15;
                    this.t += 0.75;
                    for (let i = 0; i <= n; i++) {
                        const z = (i * n + this.t % n);
                        const y = w * n / z + 32;
                        Graphics.drawLine(0, y, w, y, PICO8_COLORS[gridcolor]);
                        const v = i + (this.t % n) / n - n / 2;
                        Graphics.drawLine(v * 9 + 64, 40, v * 60 + 64, w, PICO8_COLORS[gridcolor] || 0xfe);
                    }
                }
            },
            flight: {
                color: PICO8_COLORS[0],
                mfg: PICO8_COLORS[7],
                mbg: PICO8_COLORS[1],
                hfg: PICO8_COLORS[4],
                pbg: PICO8_COLORS[5],
                draw: () => {
                    const n = 32;
                    const sin_t_over_300 = Math.sin(this.t / 300);
                    const t_mod_n = this.t % n;
                    const c = sin_t_over_300;
                    const sin_t_over_9 = c / 9;
                    const sin_t_over_3 = c / 3;
                    const const1 = sin_t_over_3 + 1;

                    const make = (y, color) => {
                        for (let i = 0; i <= n; i++) {
                            const z = (i * n + t_mod_n) / 1500;
                            for (let j = -n; j <= n; j++) {
                                const x = ((j * 0.7 + c) / z) + y;
                                const y_pos = ((-j * sin_t_over_9 + const1) / z) + y;
                                Graphics.putPixel(x, y_pos, PICO8_COLORS[color] || 0xfe);
                            }
                        }
                    };
                    this.t -= 2;
                    make(5, 8);
                    make(40, 9);
                    make(70, 10);
                }
            },
            matrix: {
                color: PICO8_COLORS[0],
                mfg: PICO8_COLORS[7],
                mbg: PICO8_COLORS[3],
                hfg: PICO8_COLORS[11],
                pbg: PICO8_COLORS[3],
                draw: () => {
                    for (let j = 3; j <= 99; j++) {
                        let c = 11;
                        const x = j * 593;
                        for (let y = 1; y <= x % c; y++) {
                            const n = (Math.pow(j, y) % 7) + 1;
                            const char = "5&y$%z?$*".charAt(n - 1);
                            text(
                                char,
                                x % 126,
                                (this.t + x) * j / 8 % 256 - y * 6,
                                PICO8_COLORS[c] || 0xfe
                            );
                            c = 3;
                        }
                    }
                    this.t += 0.7;
                }
            },
            starfield: {
                color: PICO8_COLORS[0],
                mfg: PICO8_COLORS[7],
                mbg: PICO8_COLORS[1],
                hfg: PICO8_COLORS[4],
                pbg: PICO8_COLORS[5],
                load: () => {
                    this.stars = [];
                    this.spots = [];
                },
                update: () => {
                    // Add new stars
                    for (let i = 0; i < 2; i++) {
                        if (this.stars.length < 100) {

                            let star = {
                                x: this.star_sx,
                                y: this.star_sy,
                                d: Math.random(),
                                spd: i + 1
                            };

                            // Let's make sure that the star is not too close to the center, need to add +- 32 to the x and +- 16 to the y
                            // The center is 64, 64

                            let center_x = 64;
                            let center_y = 64;

                            let distance = Math.sqrt(Math.pow(star.x - center_x, 2) + Math.pow(star.y - center_y, 2));

                            while (distance < 32) {
                                star.x = Math.random() * 128;
                                star.y = Math.random() * 128;
                                distance = Math.sqrt(Math.pow(star.x - center_x, 2) + Math.pow(star.y - center_y, 2));
                            }

                            this.stars.push({
                                x: this.star_sx,
                                y: this.star_sy,
                                d: Math.random(),
                                spd: i + 1
                            });
                        }
                    }

                    // Update stars
                    this.stars = this.stars.filter(s => {
                        s.d += 1;
                        s.x += Math.cos(s.d * Math.PI * 2) * s.spd;
                        s.y += Math.sin(s.d* Math.PI * 2) * s.spd;

                        if (s.d > this.star_cutoff - s.spd) {
                            for (let i = 1; i <= Math.floor(s.spd); i++) {
                                this.spots.push({
                                    x: s.x - Math.cos(s.d * Math.PI * 2) * s.spd / i,
                                    y: s.y - Math.sin(s.d * Math.PI * 2) * s.spd / i,
                                    d: s.spd * 2
                                });
                            }
                        }

                        return !(s.x < -32 || s.x > 128 || s.y < -16 || s.y > 128);
                    });

                    // Update spots
                    this.spots = this.spots.filter(s => {
                        s.d -= 1;
                        return s.d >= 0;
                    });
                },
                draw: () => {
                    // Draw stars
                    this.stars.forEach(s => {
                        Graphics.putPixel(s.x, s.y, PICO8_COLORS[7]);
                    });

                    // Draw spots
                    this.spots.forEach(s => {
                        Graphics.putPixel(s.x, s.y, PICO8_COLORS[5]);
                    });
                }
            }
        };
    }

    createStar() {
        const angle = Math.random() * Math.PI * 2;
        return {
            x: this.star_sx,
            y: this.star_sy,
            d: angle,
            spd: Math.random() * 0.5 + 0.1  // Slower speed
        };
    }

    load(stageId) {
        let bgp = this.list[stageId];

        this.current = this.backgroundTypes[bgp];
        this.currentName = bgp;

        this.rand = [];
        this.rand2 = [];
        this.t = 0;

        if (this.current && this.current.load) {
            this.current.load();
        }
    }

    next() {
        this.load((this.list.indexOf(this.currentName) + 1) % this.list.length);
    }

    update() {
        this.t += 0.01;  // Adjust this value as needed
        if (this.current && this.current.update) {
            this.current.update();
        }
    }

    draw() {
        if (this.current && this.current.draw) {
            this.current.draw();
        }
    }
}
