import { Renderer } from "./renderer.js";

export class ManualRendering extends Renderer {
    constructor(canvas, w, h) {
        super(canvas, w, h);
        this.ctx = this.canvas.getContext("2d");
        this.ctx.imageSmoothingEnabled = false;
        this.backCanvas = new OffscreenCanvas(w, h);
        this.backCtx = this.backCanvas.getContext("2d");
        this.backCtx.imageSmoothingEnabled = false;
        this.backImageData = new ImageData(w, h);
    }

    clear_background(r, g, b, a) {
        for (let i = 0; i < this.backImageData.data.length; i += 4) {
            this.backImageData.data[i + 0] = r;
            this.backImageData.data[i + 1] = g;
            this.backImageData.data[i + 2] = b;
            this.backImageData.data[i + 3] = a;
        }
    }

    draw_rectangle(r, g, b, a, start_x, start_y, w, h) {
        for (let y = 0; y < h; y++) {
            for (let x = 0; x < w; x++) {
                const idx = ((start_y + y) * this.w + (start_x + x)) * 4;
                this.backImageData.data[idx + 0] = r;
                this.backImageData.data[idx + 1] = g;
                this.backImageData.data[idx + 2] = b;
                this.backImageData.data[idx + 3] = a;
            }
        }
    }

    draw_circle(r, g, b, a, start_x, start_y, rad) {
        for (let y = start_y - rad; y <= start_y + rad; y++) {
            if (y < 0 || y >= this.h) continue;
            for (let x = start_x - rad; x <= start_x + rad; x++) {
                if (x < 0 || x >= this.w) continue;
                const dx = x - start_x;
                const dy = y - start_y;
                if (dx * dx + dy * dy <= rad * rad) {
                    const idx = (y * this.w + x) * 4;
                    if (idx >= 0 && idx < this.backImageData.data.length) {
                        this.backImageData.data[idx + 0] = r;
                        this.backImageData.data[idx + 1] = g;
                        this.backImageData.data[idx + 2] = b;
                        this.backImageData.data[idx + 3] = a;
                    }
                }
            }
        }
    }

    render() {
        this.backCtx.putImageData(this.backImageData, 0, 0);
        this.ctx.drawImage(this.backCtx.canvas, 0, 0, this.w, this.h);
    }
}
