export class Mouse_ {
    #listeners = new Map();
    #state = {
        x: 0,
        y: 0,
        left: false,
        right: false
    };

    constructor() {
        this.#listeners.set('move', []);
        this.#listeners.set('leftDown', []);
        this.#listeners.set('leftUp', []);
    }

    get coords() {
        return {x: this.#state.x, y: this.#state.y};
    }

    addEventListener(event, callback) {
        this.#listeners.set(event, [...this.#listeners.get(event), callback]);

        return () => {
            this.removeEventListener(event, callback);
        }
    }

    removeEventListener(event, callback) {
        this.#listeners.set(event, this.#listeners.get(event).filter(listener => listener !== callback));
    }

    #emit(event, data) {
        const listeners = this.#listeners.get(event);
        if (listeners) {
            listeners.forEach(listener => listener(data));
        }
    }

    mouseUpdate(mouseState) {
        const x = mouseState.get('x');
        const y = mouseState.get('y');
        const left = mouseState.get('left');
        const right = mouseState.get('right');

        if (x !== this.#state.x || y !== this.#state.y) {
            this.#state.x = x;
            this.#state.y = y;
            this.#emit('move', {x, y});
        }

        if (left !== this.#state.left) {
            this.#state.left = left;
            this.#emit(left ? 'leftDown' : 'leftUp', {x, y});
        }

        if (right !== this.#state.right) {
            this.#state.right = right;
            this.#emit(right ? 'rightDown' : 'rightUp', {x, y});
        }
    }
}

export const Mouse = new Mouse_();