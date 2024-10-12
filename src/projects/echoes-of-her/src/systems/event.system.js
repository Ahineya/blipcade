/**
 * @property type {string}
 * @property data {any}
 */
export class Evt {
    constructor(type, data) {
        this.type = type;
        this.data = data;
    }
}

// TODO: This event system messes up the game loop, making systems run callbacks not in the systems order.
// We need to make it so that events are processed through ECS, and that systems can listen to events.
class EventSystem {
    constructor() {
        this.events = [];
        this.listeners = [];
    }
    update() {
        this.events = this.events.map(event => {
            event.time -= 1;
            return event;
        }).filter(event => event.time > 0)
    }

    processEvents(systemId) {
        this.listeners.filter(l => l.systemId === systemId).forEach(listener => {
            this.events.forEach(event => {
                if (event.type === listener.eventType) {
                    if (listener.callback(event)) {
                        event.time = 0;
                        event.handled = true;
                    }
                }
            });
        });
    }

    /**
     * @param eventType {string}
     * @param callback {function(Evt): boolean}
     * @param [systemId=""] {string}
     * @param [id=""] {string}
     */
    onEvent(eventType, callback, systemId = "", id = "") {
        this.listeners.push({
            eventType,
            callback,
            systemId,
            id
        });
    }

    /**
     * @param id {string}
     */
    removeListenersWithId(id) {
        this.listeners = this.listeners.filter(listener => listener.id !== id);
    }

    emit(event) {
        this.events.push({
            ...event,
            time: 2,
            handled: false
        });
    }
}



export const eventSystem = new EventSystem();