export class FileManager {
    currentDir = fs.getCartsDir();

    constructor() {
    }

    ls() {
        return fs.list(this.currentDir);
    }

    cd(path) {
        this.currentDir = fs.resolvePath(path);
    }

    cdRelative(path) {
        this.cd(this.currentDir + '/' + path);
    }

    updir() {
        this.cdRelative('..');
    }

    load(cartPath) {
        loadCart(cartPath);
    }

    home() {
        this.currentDir = fs.getCartsDir();
    }
}
