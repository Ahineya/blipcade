import {sc} from "../../framework/scene";
import {w} from "../../framework/widget";
import {B} from "../../framework/b";
import {Button} from "../../framework/components/button";
import {state} from "../../state/state";
import {CurrentSpriteData} from "../current-sprite-data";
import {SpritesheetViewer} from "../spritesheet-viewer";
import {spriteViewer} from "./sprite-viewer";
import {divider} from "./divider";
import {spriteControls} from "./sprite-edit/sprite-controls";
import {Text} from "../../framework/components/text";

export const main = (x, y) => {
    return sc(x, y, [ // Main scene
        spriteViewer(0, 0),
        divider(B.SCREEN_SIZE.width / 2 - 5, 0 - 4),
        spriteControls(B.SCREEN_SIZE.width / 2, 0),

        sc(B.SCREEN_SIZE.width / 2, B.SCREEN_SIZE.height / 2 - 12 - 12, [ // Sprite navigation scene
            new Button(0, 0, 'prev', 13, 13, () => state.prevSprite()),
            new Button(32, 0, 'next', 13, 13, () => state.nextSprite()),
            new CurrentSpriteData(68, 2, "index/total"),
        ]),
        w(0 - 4, B.SCREEN_SIZE.height / 2 - 11, (x, y) => {
            drawLine(x, y, x + B.SCREEN_SIZE.width, y, 13);
        }),
        new SpritesheetViewer(0, B.SCREEN_SIZE.height / 2 - 6),
        // sc(0, B.SCREEN_SIZE.height - 64, [ // Text demo scene
        //     // Characters: " !\"#$%&'()*+,-./0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\\]^_`abcdefghijklmnopqrstuvwxyz{|}~"
        //     new Text(0, 0, 'A QUICK BROWN FOX JUMPS OVER THE LAZY DOG'),
        //     new Text(0, 12, '0123456789 !@#$%^&*'),
        //     new Text(0, 24, '()_+{}[]|\\;:\'",.<>?/'),
        //     new Text(0, 36, 'abcdefghijklmnopqrstuvwxyz'),
        // ]),
    ])
}
