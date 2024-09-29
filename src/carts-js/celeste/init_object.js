import {state} from "./state";

export function init_object(type, x, y) {
    let obj = new type(x, y);
    state.objects.push(obj);
    return obj;
}