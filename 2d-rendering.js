import { Renderer } from "./renderer.js";

export class TwoDRendering extends Renderer {
    constructor(canvas, w, h) {
        super(canvas, w, h);
        /** @type {CanvasRenderingContext2D} */
        this.ctx = this.canvas.getContext("2d");
    }

    /** @override */
    clear_background(r, g, b, a) {
        this.ctx.clearRect(0, 0, this.w, this.h);
        this.draw_rectangle(r, g, b, a, 0, 0, this.w, this.h);
    }

    /** @override */
    draw_rectangle(r, g, b, a, start_x, start_y, w, h) {
        this.ctx.fillStyle = `rgba(${r},${g},${b},${a})`;
        this.ctx.fillRect(start_x, start_y, w, h);
    }

    /** @override */
    draw_circle(r, g, b, a, start_x, start_y, rad) {
        this.ctx.fillStyle = `rgba(${r},${g},${b},${a})`;
        this.ctx.beginPath();
        this.ctx.arc(start_x, start_y, rad, 0, Math.PI*2);
        this.ctx.closePath();
        this.ctx.fill();
    }

    /** @override */
    render() {}
}
