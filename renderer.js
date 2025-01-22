export class Renderer {
    constructor(canvas, w, h) {
        this.canvas = canvas;
        this.w = w;
        this.h = h;
        this.canvas.width = w;
        this.canvas.height = h;
    }
    clear_background(r, g, b, a) { }
    draw_rectangle(r, g, b, a, start_x, start_y, w, h) { }
    draw_circle(r, g, b, a, start_x, start_y, rad) { }
}
