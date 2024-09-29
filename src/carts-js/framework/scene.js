import {Widget} from "./widget";

export class Scene extends Widget {
    constructor(x = 0, y = 0) {
        super(x, y);
    }

    render() {
        for (let child of this.children) {
            child.render();
        }
    }

    update() {
        for (let child of this.children) {
            child.update();
        }
    }
}

export function sc(...args) {
    if (args.length === 0) {
        return new Scene(0, 0);
    }

    if (args.length === 1) {
        return args[0];
    }

    if (args.length === 2) {
        const [scene, children] = args;
        children.forEach(child => scene.addChild(child));
        scene.updateChildrenGlobalPositions();
        return scene;
    }

    if (args.length === 3) {
        const [x, y, children] = args;
        const scene = new Scene(x, y);
        children.forEach(child => {
            scene.addChild(child);
        });
        scene.updateChildrenGlobalPositions();
        return scene;
    }
}