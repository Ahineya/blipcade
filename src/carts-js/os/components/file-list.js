import {state} from "../state/state";
import {Widget} from "../../framework/widget";
import {Button} from "../../framework/components/button";
import {hexToColor} from "../../framework/colors";

export class FileList extends Widget {
    files = [];
    dir = '';
    buttons = [];
    upButton = null;

    constructor(x = 0, y = 0) {
        super(x, y);

        this.upButton = new Button({
            x: this.globalX + 4,
            y: this.globalY + 8 + global.GLYPH_SIZE.height,
            text: "..",
            color: hexToColor("#000000"),
            border: false,
            hoverColor: hexToColor("#474747"),
            onClick: () => {
                state.fileManager.updir();
            },
            textColor: hexToColor("#ffffff")
        });

        this.homeButton = new Button({
            x: this.globalX + 24,
            y: this.globalY + 8 + global.GLYPH_SIZE.height,
            text: "home",
            color: hexToColor("#000000"),
            border: false,
            hoverColor: hexToColor("#474747"),
            onClick: () => {
                state.fileManager.home();
            },
            textColor: hexToColor("#ffffff")
        });
    }

    update() {
        super.update();

        if (this.dir !== state.fileManager.currentDir) {
            this.dir = state.fileManager.currentDir;
            this.files = state.fileManager.ls()
                .filter(file => file.ext === 'blipcart' || file.is_dir)
                .sort((a, b) => {
                    if (a.is_dir && !b.is_dir) {
                        return -1;
                    }
                    if (!a.is_dir && b.is_dir) {
                        return 1;
                    }
                    return a.name.localeCompare(b.name);
                });

            this.buttons.forEach(button => button.destroy());

            this.buttons = this.files
                .map((file, index) => {
                    return new Button({
                        x: this.globalX,
                        y: this.globalY + 4 + (global.GLYPH_SIZE.height + 4) * 2 + 10 * index,
                        text: file.name,
                        color: hexToColor("#000000"),
                        border: false,
                        hoverColor: hexToColor("#474747"),
                        onClick: () => {
                            if (file.is_dir) {
                                state.fileManager.cdRelative(file.name);
                            } else {
                                state.fileManager.load(file.path);
                            }
                        },
                        textColor: file.is_dir ? hexToColor("#ffffff") : hexToColor("#38b838")
                    });
                });
        }
    }

    render() {
        text(this.globalX, this.globalY, state.fileManager.currentDir, 0xfe);

        this.upButton.render();
        this.homeButton.render();
        this.buttons.forEach(button => button.render());
    }
}
