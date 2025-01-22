import { ManualRendering } from "./manual-rendering.js";
import { TwoDRendering } from "./2d-rendering.js";
import { Renderer } from "./renderer.js";

const WIDTH = 7*180;
const HEIGHT = 4*180;

/** @type {number} */
let previous = undefined;
let wasm = undefined;

/** @type {Renderer} */
let renderer = undefined;

/** @type {number} */
let dt = undefined;

let doUpdate = true;
let skipNextUpdate = false;

document.addEventListener("visibilitychange", ()=>{
    doUpdate = !document.hidden;
    if (!doUpdate) skipNextUpdate = true;
});

WebAssembly.instantiateStreaming(fetch("main.wasm"), {
    env: make_env({
        TraceLog: (level, format, text_ptr) => {
            const buffer = wasm.instance.exports.memory.buffer;
            const text = cstr_by_ptr(buffer, text_ptr);
            console.log(text);
        },
        GameOver: ()=>{
            window.alert("Game Over! Restart?");
            location.reload();
        },
        RandomInt: (min, max) => {
            return Math.floor(Math.random() * (max - min)) + min;
        },
        BeginDrawing: ()=>{},
        EndDrawing: ()=>{},
        InitWindow: (width, height, text_ptr) => {
            const buffer = wasm.instance.exports.memory.buffer;
            const text = cstr_by_ptr(buffer, text_ptr);
            document.title = text;
        },
        ClearBackground: (color_ptr)=>{
            const buffer = wasm.instance.exports.memory.buffer;
            const rgba = new Uint8Array(buffer, color_ptr, 4);
            renderer.clear_background(...rgba);
        },
        DrawRectangle: (start_x, start_y, w, h, color_ptr) => {
            const buffer = wasm.instance.exports.memory.buffer;
            const rgba = new Uint8Array(buffer, color_ptr, 4);
            renderer.draw_rectangle(...rgba, start_x, start_y, w, h);
        },
        DrawCircle: (start_x, start_y, rad, color_ptr) => {
            const buffer = wasm.instance.exports.memory.buffer;
            const rgba = new Uint8Array(buffer, color_ptr, 4);
            renderer.draw_circle(...rgba, start_x, start_y, rad);
        },
    }),
}).then((wasmModule) => {
    console.log("WASM instantiated", wasmModule);
    var params = new URLSearchParams(window.location.search);

    wasm = wasmModule;
    /** @type {HTMLCanvasElement} */
    const canvas = document.getElementById("canvas");

    let target_renderer = params.get("renderer") ?? "manual";
    switch(target_renderer) {
        case "2d":
            renderer = new TwoDRendering(canvas, WIDTH, HEIGHT);
            break;
        case "manual":
            renderer = new ManualRendering(canvas, WIDTH, HEIGHT);
            break;
        default:
            throw new Error(`Renderer ${target_renderer} not supported. Try 'manual' or '2d' or 'webgl'`);
    }
    if (!renderer) {
        throw new Error("Failed to initalize renderer");
    }

    window.addEventListener("keydown", (e)=>{
        wasm.instance.exports.game_keydown(e.keyCode);
    });

    wasm.instance.exports.game_init(params.get("debug")?.includes("hitbox") ?? false);
    window.requestAnimationFrame(first);
}).catch((e) => {
    console.error("Failed to instantiate WASM", e);
});

function first(timestamp) {
    previous = timestamp;
    window.requestAnimationFrame(next);
}

function next(timestamp) {
    dt = (timestamp - previous)*0.001;
    previous = timestamp;
    if (doUpdate){
        if (!skipNextUpdate) {
            wasm.instance.exports.game_update(dt);
            renderer.render();
        } else {
            skipNextUpdate = false;
        }
    }
    window.requestAnimationFrame(next);
}

function make_env(...envs) {
    return new Proxy(envs, {
        get(target, prop, receiver) {
            for (let env of envs) {
                if (env.hasOwnProperty(prop)) {
                    return env[prop];
                }
            }
            return (...args) => {
                console.error("NOT IMPLEMENTED: " + prop, args);
            }
        }
    });
}

function cstr_by_ptr(mem_buffer, ptr) {
    const mem = new Uint8Array(mem_buffer);
    const len = cstrlen(mem, ptr);
    const bytes = new Uint8Array(mem_buffer, ptr, len);
    return new TextDecoder().decode(bytes);
}

function cstrlen(mem, ptr) {
    let len = 0;
    while (mem[ptr] != 0) {
        len++;
        ptr++;
    }
    return len;
}

function color_hex_unpacked(r, g, b, a) {
    r = r.toString(16).padStart(2, "0");
    g = g.toString(16).padStart(2, "0");
    b = b.toString(16).padStart(2, "0");
    a = a.toString(16).padStart(2, "0");
    return `#${r}${g}${b}${a}`;
}

function color_hex(color) {
    const r = ((color>>(0*8))&0xFF).toString(16).padStart(2, "0");
    const g = ((color>>(1*8))&0xFF).toString(16).padStart(2, "0");
    const b = ((color>>(2*8))&0xFF).toString(16).padStart(2, "0");
    const a = ((color>>(3*8))&0xFF).toString(16).padStart(2, "0");
    return `#${r}${g}${b}${a}`;
}
