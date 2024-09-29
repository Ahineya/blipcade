class State {}

class CelesteState extends State {
    constructor() {
        super();
        this.room = { x: 7, y: 3 };
        this.objects = [];
        this.freeze = 0;
        this.shake = 0;
        this.will_restart = false;
        this.delay_restart = 0;
        this.got_fruit = Array(30).fill(false);
        this.has_dashed = false;
        this.sfx_timer = 0;
        this.has_key = false;
        this.pause_player = false;
        this.flash_bg = false;
        this.music_timer = 0;
        this.max_djump = 1;
        this.start_game = false;
        this.start_game_flash = 0;
        this.frames = 0;
        this.seconds = 0;
        this.minutes = 0;
        this.deaths = 0;
        this.clouds = [
            { x: 0, y: 0, w: 64, spd: 0.5 },
            { x: 0, y: 0, w: 32, spd: 0.3 },
            { x: 0, y: 0, w: 16, spd: 0.1 },
        ];
        this.particles = [];
        this.dead_particles = [];
    }
}

export const state = new CelesteState();