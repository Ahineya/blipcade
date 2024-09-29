import {state} from "./state";
import {Fruit} from "./fruit";
import {FakeWall} from "./fakewall";
import {RoomTitle} from "./room-title";
import {is_title, level_index, tile_at} from "./helpers";
import {Platform} from "./platform";
import {Spring} from "./spring";
import {Balloon} from "./balloon";
import {FallFloor} from "./fallfloor";
import {FlyFruit} from "./fly-fruit";
import {Key} from "./key";
import {Chest} from "./chest";
import {Message} from "./message";
import {BigChest} from "./big-chest";
import {Flag} from "./flag";
import {PlayerSpawn} from "./player-spawn";

export function load_room(x, y) {
    state.has_dashed = false;
    state.has_key = false;

    // Remove existing objects
    state.objects = [];

    // Set current room
    state.room = {x, y};

    log("Loading room " + x + ", " + y);

    // Create tiles and objects
    for (let tx = 0; tx < 16; tx++) {
        for (let ty = 0; ty < 16; ty++) {
            let tile = tile_at(tx, ty);
            if (tile === 1) { // Player spawn point
                log("Player spawn point at " + tx + ", " + ty);
                state.objects.push(new PlayerSpawn(tx * 8, ty * 8));
            } else if (tile === 11) { // Left moving platform
                state.objects.push(new Platform(tx * 8, ty * 8, -1));
            } else if (tile === 12) { // Right moving platform
                state.objects.push(new Platform(tx * 8, ty * 8, 1));
            } else if (tile === 18) { // Spring
                state.objects.push(new Spring(tx * 8, ty * 8));
            } else if (tile === 22) { // Balloon
                state.objects.push(new Balloon(tx * 8, ty * 8));
            } else if (tile === 23) { // Fall Floor
                state.objects.push(new FallFloor(tx * 8, ty * 8));
            } else if (tile === 26) { // Fruit
                if (!state.got_fruit[level_index()]) {
                    state.objects.push(new Fruit(tx * 8, ty * 8));
                }
            } else if (tile === 28) { // Fly Fruit
                if (!state.got_fruit[level_index()]) {
                    state.objects.push(new FlyFruit(tx * 8, ty * 8));
                }
            } else if (tile === 64) { // Fake Wall
                if (!state.got_fruit[level_index()]) {
                    state.objects.push(new FakeWall(tx * 8, ty * 8));
                }
            } else if (tile === 8) { // Key
                if (!state.got_fruit[level_index()]) {
                    state.objects.push(new Key(tx * 8, ty * 8));
                }
            } else if (tile === 20) { // Chest
                if (!state.got_fruit[level_index()]) {
                    state.objects.push(new Chest(tx * 8, ty * 8));
                }
            } else if (tile === 86) { // Message
                state.objects.push(new Message(tx * 8, ty * 8));
            } else if (tile === 96) { // Big Chest
                state.objects.push(new BigChest(tx * 8, ty * 8));
            } else if (tile === 118) { // Flag
                state.objects.push(new Flag(tx * 8, ty * 8));
            }
        }
    }

    if (!is_title()) {
        state.objects.push(new RoomTitle());
    }
}
