export class Renderer {
    /**
    * @param {HTMLCanvasElement} canvas 
    * @param {number} w 
    * @param {number} h 
    */
    constructor(canvas, w, h) {
        this.canvas = canvas;
        this.w = w;
        this.h = h;
        this.canvas.width = w;
        this.canvas.height = h;
    }

    /**
    * @param {number} r
    * @param {number} g
    * @param {number} b
    * @param {number} a
    */
    clear_background(r, g, b, a) { }

    /**
    * @param {number} r
    * @param {number} g
    * @param {number} b
    * @param {number} a
    * @param {number} start_x 
    * @param {number} start_y 
    * @param {number} w 
    * @param {number} h 
    */
    draw_rectangle(r, g, b, a, start_x, start_y, w, h) { }

    /**
    * @param {number} r
    * @param {number} g
    * @param {number} b
    * @param {number} a
    * @param {number} start_x 
    * @param {number} start_y 
    * @param {number} rad
    */
    draw_circle(r, g, b, a, start_x, start_y, rad) { }
}
