export class Widget {
    constructor(x = 0, y = 0) {
        this.x = x;
        this.y = y;
        this.globalX = x;
        this.globalY = y;
        this.children = [];
        this.parent = null;
    }

    addChild(child) {
        this.children.push(child);
        child.parent = this;
        this.updateChildrenGlobalPositions();
    }

    updateGlobalPosition() {
        if (this.parent) {
            this.globalX = this.parent.globalX + this.x;
            this.globalY = this.parent.globalY + this.y;
        } else {
            this.globalX = this.x;
            this.globalY = this.y;
        }
        this.updateChildrenGlobalPositions();
    }

    updateChildrenGlobalPositions() {
        for (let child of this.children) {
            child.updateGlobalPosition();
        }
    }

    render() {
        // To be implemented by subclasses
    }

    update() {
        // To be implemented by subclasses
    }
}

export function w(x, y, render) {
    class W extends Widget {
        render() {
            render(this.globalX, this.globalY);
        }
    }

    return new W(x, y);
}