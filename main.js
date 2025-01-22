const WIDTH = 7*180;
const HEIGHT = 4*180;

/** @type {number} */
let previous = undefined;
let wasm = undefined;
/** @type {CanvasRenderingContext2D} */
let ctx = undefined;
/** @type {CanvasRenderingContext2D} */
let backCtx = undefined;
const backImageData = new ImageData(WIDTH, HEIGHT);

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
        BeginDrawing: ()=>{},
        EndDrawing: ()=>{},
        InitWindow: (width, height, text_ptr) => {
            const buffer = wasm.instance.exports.memory.buffer;
            const text = cstr_by_ptr(buffer, text_ptr);
            document.title = text;
        },
        ClearBackground: (color_ptr)=>{
            const buffer = wasm.instance.exports.memory.buffer;
            const [r,g,b,a] = new Uint8Array(buffer, color_ptr, 4);
            for (let i = 0; i < backImageData.data.length; i+=4){
                backImageData.data[i+0] = r;
                backImageData.data[i+1] = g;
                backImageData.data[i+2] = b;
                backImageData.data[i+3] = a;
            }
        },
        DrawRectangle: (start_x, start_y, w, h, color_ptr) => {
            const buffer = wasm.instance.exports.memory.buffer;
            const [r,g,b,a] = new Uint8Array(buffer, color_ptr, 4);
            for (let y = 0; y < h; y++){
                for (let x = 0; x < w; x++) {
                    const idx = ((start_y+y) * WIDTH + (start_x + x)) * 4;
                    backImageData.data[idx + 0] = r;
                    backImageData.data[idx + 1] = g;
                    backImageData.data[idx + 2] = b;
                    backImageData.data[idx + 3] = a;
                }
            }
        },
        DrawCircle: (start_x, start_y, rad, color_ptr) => {
            const buffer = wasm.instance.exports.memory.buffer;
            const [r,g,b,a] = new Uint8Array(buffer, color_ptr, 4);
            for (let y = start_y - rad; y <= start_y + rad; y++){
                if (y < 0 || y >= HEIGHT) continue;
                for (let x = start_x - rad; x <= start_x + rad; x++) {
                    if (x < 0 || x >= WIDTH) continue;
                    const dx = x - start_x;
                    const dy = y - start_y;
                    if (dx * dx + dy * dy <= rad*rad) {
                        const idx = (y * WIDTH + x) * 4;
                        if (idx >= 0 && idx < backImageData.data.length) {
                            backImageData.data[idx + 0] = r;
                            backImageData.data[idx + 1] = g;
                            backImageData.data[idx + 2] = b;
                            backImageData.data[idx + 3] = a;
                        }
                    }
                }
            }
        },
    }),
}).then((wasmModule) => {
    console.log("WASM instantiated", wasmModule);
    wasm = wasmModule;
    /** @type {HTMLCanvasElement} */
    const canvas = document.getElementById("canvas");
    canvas.width = WIDTH;
    canvas.height = HEIGHT;
    ctx = canvas.getContext("2d");
    ctx.imageSmoothingEnabled = false;
    const backCanvas = new OffscreenCanvas(WIDTH, HEIGHT);
    backCtx = backCanvas.getContext("2d");
    backCtx.imageSmoothingEnabled = false;

    window.addEventListener("keydown", (e)=>{
        wasm.instance.exports.game_keydown(e.keyCode);
    });

    var params = new URLSearchParams(window.location.search);
    wasm.instance.exports.game_init(params.getAll("debug").includes("hitbox"));
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
            backCtx.putImageData(backImageData, 0, 0);
            ctx.drawImage(backCtx.canvas, 0, 0, ctx.canvas.width, ctx.canvas.height);
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
