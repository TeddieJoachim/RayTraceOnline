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

  const delTime = 1.0/60.0; 
  let prevTime = 0;
  let accumulator = 0.0;

  // currTime is supplied by requestAnimationFrame
  function gameLoop(currTime) {
    // convert milliseconds to seconds 
    const frameTime = (currTime - prevTime) / 1000.0;
    prevTime = currTime;

    // accumulate elapsed time. if rendering fluctuates between FPS, this keeps simulation consistent
    accumulator += Math.min(frameTime, 0.25);
    // we run simulation at fixed intervals
    while (accumulator >= delTime) {
      Module._advanceBoids(delTime); // Update shared heap memory
      accumulator -= delTime;
    }

    
    // render using sharedHeapCoords (this is a pointer, not a copy!)
    renderer.render(sharedHeapCoords, numBoids);

    // next frame
    requestAnimationFrame(gameLoop);
  }

  requestAnimationFrame(gameLoop);
}

startGame(); 