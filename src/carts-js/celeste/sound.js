export function pmusic(number, fadems, channelMask) {
    // If number == -1, stop music. Fade out over fadems milliseconds.
    // If number > 0, play music track number. Fade in over fadems milliseconds. If number == 0, don't fade in.
    // channelMask is a bitmask of which channels to reserve for music. If 0, use the default music channels.

    const PICO_MAPPING = {
        0: 0,
        10: 1,
        20: 2,
        30: 3,
        40: 4,
    }

    if (number == -1) {
        stopSound(0);
        return;
    }

    playSound(PICO_MAPPING[number], 0);
}

export function psfx(number) {
    /*
    Mapping

    0: 00 00 00 00 00 00 2D C0 00 00 00 00 00 00 00 00 assets/celeste-sound/snd0.wav
1: 00 00 2D C0 00 00 5B 80 00 00 00 00 00 00 00 00 assets/celeste-sound/snd1.wav
2: 00 00 5B 80 00 00 B7 00 00 00 00 00 00 00 00 00 assets/celeste-sound/snd13.wav
3: 00 00 B7 00 00 01 12 80 00 00 00 00 00 00 00 00 assets/celeste-sound/snd14.wav
4: 00 01 12 80 00 01 57 20 00 00 00 00 00 00 00 00 assets/celeste-sound/snd15.wav
5: 00 01 57 20 00 02 C5 20 00 00 00 00 00 00 00 00 assets/celeste-sound/snd16.wav
6: 00 02 C5 20 00 03 09 C0 00 00 00 00 00 00 00 00 assets/celeste-sound/snd2.wav
7: 00 03 09 C0 00 03 93 00 00 00 00 00 00 00 00 00 assets/celeste-sound/snd23.wav
8: 00 03 93 00 00 03 C0 C0 00 00 00 00 00 00 00 00 assets/celeste-sound/snd3.wav
9: 00 03 C0 C0 00 04 1C 40 00 00 00 00 00 00 00 00 assets/celeste-sound/snd35.wav
10: 00 04 1C 40 00 05 8A 40 00 00 00 00 00 00 00 00 assets/celeste-sound/snd37.wav
11: 00 05 8A 40 00 06 9C C0 00 00 00 00 00 00 00 00 assets/celeste-sound/snd38.wav
12: 00 06 9C C0 00 06 F8 40 00 00 00 00 00 00 00 00 assets/celeste-sound/snd4.wav
13: 00 06 F8 40 00 08 66 40 00 00 00 00 00 00 00 00 assets/celeste-sound/snd40.wav
14: 00 08 66 40 00 08 AA E0 00 00 00 00 00 00 00 00 assets/celeste-sound/snd5.wav
15: 00 08 AA E0 00 09 61 E0 00 00 00 00 00 00 00 00 assets/celeste-sound/snd50.wav
16: 00 09 61 E0 00 09 D4 40 00 00 00 00 00 00 00 00 assets/celeste-sound/snd51.wav
17: 00 09 D4 40 00 0A 18 E0 00 00 00 00 00 00 00 00 assets/celeste-sound/snd54.wav
18: 00 0A 18 E0 00 0B 14 80 00 00 00 00 00 00 00 00 assets/celeste-sound/snd55.wav
19: 00 0B 14 80 00 0B 59 20 00 00 00 00 00 00 00 00 assets/celeste-sound/snd6.wav
20: 00 0B 59 20 00 0B 86 E0 00 00 00 00 00 00 00 00 assets/celeste-sound/snd7.wav
21: 00 0B 86 E0 00 0B CB 80 00 00 00 00 00 00 00 00 assets/celeste-sound/snd8.wav
22: 00 0B CB 80 00 0C 10 20 00 00 00 00 00 00 00 00 assets/celeste-sound/snd9.wav
     */

    const PICO_MAPPING = {
        0: 0,
        1: 1,
        2: 6,
        3: 8,
        4: 12,
        5: 14,
        6: 19,
        7: 20,
        8: 21,
        9: 22,
        13: 2,
        14: 3,
        15: 4,
        16: 5,
        23: 7,
        35: 9,
        37: 10,
        38: 11,
        40: 13,
        50: 15,
        51: 16,
        54: 17,
        55: 18
    }

    // log(`Playing sound ${number} with mapping ${PICO_MAPPING[number]}`);

    playSound(PICO_MAPPING[number] + 5, 1);
}