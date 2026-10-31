// import a renderer object
import { Renderer } from "./renderer.js";
let Module; // C functions will be called through module._(a c func name)

// grab webgl rendering context
const canvas = document.getElementById("gameCanvas");
const gl = canvas.getContext("webgl2");

if (!gl) {
  console.error("Ruh roh couldn't load webgl2");
}

// async needed so that javascript doesn't just mosey along and do other shit
async function startGame() {
  Module = await createWASMModule(); // Locks things up until we get our WASM

  // if we want javascript/webgl to control the number of boids, do it here 
  Module._initializeBoids();

  runGame(Module);
  //Module._freeBoids();                                                                  FREE THEM BOIDS WHEN YOU'RE DONE
}


function runGame(Module) {
  const numBoids = Module._getNumBoids();
  const boidDim = Module._getDim();
  const numPositions = numBoids * boidDim;
  const renderer = new Renderer(canvas, boidDim);
  // Module.HEAPU8.buffer is the heap memory of WebAssembly. 
  // Module._getBoidPos returns the address we begin reading  
  // we read numPositions (bytes * sizeof(float)) = 4 bytes * numFloats
  const sharedHeapCoords = new Float32Array(Module.HEAPU8.buffer, Module._getBoidPositions(), numPositions);

  // TODOS include making our simulation domain, and initializing the boids to random positions in this domain.
  // eventually, have to account for camera and all that shit, gonna get rowdy

  function gameLoop() {
    // Update shared heap memory
    Module._advanceBoids();
    console.log(sharedHeapCoords);

    // render using sharedHeapCoords (this is a pointer, not a copy!)
    renderer.render(sharedHeapCoords, numBoids);

    // next frame
    requestAnimationFrame(gameLoop);
  }

  requestAnimationFrame(gameLoop);
}

startGame();