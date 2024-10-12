class TimerSystem {
    constructor() {
        this.entity = ECS.createEntity();
        ECS.addComponent(this.entity, "Timer", {
            timers: []
        });
        ECS.addComponent(this.entity, "Tag", "Timer");
        ECS.addComponent(this.entity, "Persistent", {});
    }

    update() {
        ECS.forEachEntity(["Timer"], (entity, timer) => {
            timer.timers = timer.timers.map(t => {
                t.time -= 1;
                return t;
            }).filter(t => t.time >= 0);
        });
    }

    addTimer(id, time) {
        ECS.forEachEntity(["Timer"], (entity, timer) => {
            timer.timers.push({
                id,
                time,
            });
        });
    }

    removeTimer(id) {
        ECS.forEachEntity(["Timer"], (entity, timer) => {
            timer.timers = timer.timers.filter(t => t.id !== id);
        });
    }

    isTimerDone(id) {
        let done = false;
        ECS.forEachEntity(["Timer"], (entity, timer) => {
            done = timer.timers.some(t => t.id === id && t.time === 0);
        });
        return done;
    }
}

export const timerSystem = new TimerSystem();