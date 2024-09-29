import {sc} from "../../../framework/scene";
import {Button} from "../../../framework/components/button";
import {state} from "../../../state/state";
import {Text} from "../../../framework/components/text";
import {CurrentSpriteData} from "../../current-sprite-data";
import {Toggle} from "../../../framework/components/toggle";

export const spriteControls = (x, y) => {
    return sc(x, y, [ // Sprite edit scene
        new Button(120, 0, 'add', 13, 13, () => {
            state.sprites.push({x: 0, y: 0, width: 8, height: 8, flags: 0});
            state.activeSprite = state.sprites.length - 1;
        }),
        new Button(120, 16, 'del', 13, 13, () => {
            state.sprites.splice(state.activeSprite, 1);
            state.activeSprite = Math.max(0, state.activeSprite - 1);
        }),
        new Button(120, 32, 'pico-8', 13, 13, () => {
            // Clear all sprites
            state.activeSprite = 0;
            state.sprites = [];
            for (let i = 0; i < 256; i++) {
                let x = (i % 16) * 8;
                let y = Math.floor(i / 16) * 8;
                state.sprites.push({x, y, width: 8, height: 8, flags: 0});
            }
        }),
        sc(0, 0, [
            new Text(0, 0, 'Width'),
            new Button(45, 0, '-', 13, 13, () => state.decSpriteWidth()),
            new Button(45 + 12, 0, '+', 13, 13, () => state.incSpriteWidth()),
            new CurrentSpriteData(45 + 12 + 16, 1, "width"),
        ]),
        sc(0, 12, [
            new Text(0, 0, 'Height'),
            new Button(45, 0, '-', 13, 13, () => state.decSpriteHeight()),
            new Button(45 + 12, 0, '+', 13, 13, () => state.incSpriteHeight()),
            new CurrentSpriteData(45 + 12 + 16, 1, "height"),
        ]),
        sc(0, 24, [
            new Text(0, 0, 'X'),
            new Button(45, 0, '<', 13, 13, () => state.decSpriteX()),
            new Button(45 + 12, 0, '>', 13, 13, () => state.incSpriteX()),
            new CurrentSpriteData(45 + 12 + 16, 1, "x"),
        ]),

        sc(0, 36, [
            new Text(0, 0, 'Y'),
            new Button(45, 0, '^', 13, 13, () => state.decSpriteY()),
            new Button(45 + 12, 0, 'v', 13, 13, () => state.incSpriteY()),
            new CurrentSpriteData(45 + 12 + 16, 1, "y"),
        ]),

        sc(0, 52, [
            new Text(0, 0, 'Flags'),
            ...Array.from({length: 8}, (_, i) => new Toggle(45 + i * 8, 0, 13, 13, (isToggled) => {
                if (!state.getActiveSprite()) {
                    return;
                }

                if (isToggled) {
                    state.getActiveSprite().flags |= (1 << i);
                } else {
                    state.getActiveSprite().flags &= ~(1 << i);
                }
            }, () => (state.getActiveSprite()?.flags & (1 << i)) > 0)),
        ]),

        sc(0, 64, [ // Zoom scene
            new Text(0, 0, 'Zoom'),
            new Toggle(45, 0, 13, 13, (isToggled) => {
                state.zoom = 1;
            }, () => state.zoom === 1),
            new Toggle(45 + 8, 0, 13, 13, (isToggled) => {
                state.zoom = 2;
            }, () => state.zoom === 2),
            new Toggle(45 + 16, 0, 13, 13, (isToggled) => {
                state.zoom = 3;
            }, () => state.zoom === 3),
        ]),
    ]);
}