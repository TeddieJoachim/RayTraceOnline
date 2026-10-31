/* Renderer is responsible for a lot. In here, we will compile the shaders, link them into 
a shader program, create a vertex buffer, place vertices into it, tell WebGl how to 
interpret this data, then draw the shape. */

import { vertexShaderSource, fragmentShaderSource } from "./shaders.js";
import { createBirdVertices } from "./primitives.js"; // TODO import createBird here lol


export class Renderer {
  constructor(canvas, boidDim) {
    this.gl = canvas.getContext("webgl2");

    if (!this.gl) {
      throw new Error("Ruh roh, renderer cannot load WebGl2");
    }

    const gl = this.gl;

    // compile shaders
    const vertexShader = this.compileShader(gl.VERTEX_SHADER, vertexShaderSource);
    const fragmentShader = this.compileShader(gl.FRAGMENT_SHADER, fragmentShaderSource);

    this.program = gl.createProgram();

    gl.attachShader(this.program, vertexShader);
    gl.attachShader(this.program, fragmentShader);
    gl.linkProgram(this.program);

    // check if linking was succesful
    if (!gl.getProgramParameter(this.program, gl.LINK_STATUS)) {
      throw new Error(gl.getProgramInfoLog(this.program));
    }

    // vertex buffer creation. vao = vertex array object
    this.vao = gl.createVertexArray();
    gl.bindVertexArray(this.vao);

    // operations that target ARRAY_BUFFER will write to this buffer
    this.buffer = gl.createBuffer();
    gl.bindBuffer(gl.ARRAY_BUFFER, this.buffer);

    // connect vertex data and aPosition. this function asks where is aPosition in the shader program
    // returns an int representing its attribute location 
    const positionLocation = gl.getAttribLocation(this.program, "aPosition");

    // causes webgl to feed vertex data to aPosition 
    gl.enableVertexAttribArray(positionLocation);

    // positionLocation -> aPosition receives data, boidDim -> boidDim values per vertex, gl.FLOAT -> they are floats, false -> don't normalize them, stride and start = 0.
    gl.vertexAttribPointer(positionLocation, boidDim, gl.FLOAT, false, 0, 0);

    // unbind our vao. A VAO is a saved description of how vertex data is organized and connected to shader inputs. 
    // Unbinding it doesn't destroy that description it just stops making it the active configuration.
    gl.bindVertexArray(null);
  }

  compileShader(type, source) {
    const gl = this.gl;
    const shader = gl.createShader(type);

    gl.shaderSource(shader, source);
    gl.compileShader(shader);

    if (!gl.getShaderParameter(shader, gl.COMPILE_STATUS)) {
      throw new Error(gl.getShaderInfoLog(shader));
    }

    return shader;
  }

  render(sharedHeapCoords, numBoids) {
    const gl = this.gl;

    gl.viewport(0, 0, gl.canvas.width, gl.canvas.height);

    gl.clearColor(0.1, 0.1, 0.1, 1.0);
    gl.clear(gl.COLOR_BUFFER_BIT);

  
    // give position to GPU 
    gl.bindBuffer(gl.ARRAY_BUFFER, this.buffer);
    gl.bufferData(gl.ARRAY_BUFFER, sharedHeapCoords, gl.DYNAMIC_DRAW);

    gl.useProgram(this.program);
    gl.bindVertexArray(this.vao);

    gl.drawArrays(gl.POINTS, 0, numBoids);
  }
}