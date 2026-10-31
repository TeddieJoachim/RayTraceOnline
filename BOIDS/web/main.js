let Module;

// async needed so that javascript doesn't just mosey along and do other shit
async function startGame() {
  Module = await createWASMModule(); // Locks things up until we get our WASM

  Module._initializeBoids();

  runGame(Module);
  Module._freeBoids();
}


function runGame(Module) {
  const numBoids = Module._getNumBoids(); // again we can ask or we can tell
  const boidDim = Module._getDim(); // ditto above
  const numPositions = numBoids * boidDim;
  // Module.HEAPU8.buffer is the heap memory of WebAssembly. 
  // Module._getBoidPos returns the address we begin reading  
  // we read numPositions (bytes * sizeof(float)) = 4 bytes * numFloats
  const sharedHeapCoords = new Float32Array(Module.HEAPU8.buffer, Module._getBoidPositions(), numPositions);

  // TODOS include making our simulation domain, and initializing the boids to random positions in this domain.
  // For now, I would like to render a single Boid as a sphere or triangle at (0,0,0)

  function gameLoop() {
    Module._advanceBoids();

    // read from sharedHeapCoords 
    console.log(sharedHeapCoords[3]);

    // render using sharedHeapCoords

    // next frame
    requestAnimationFrame(gameLoop);
  }

  requestAnimationFrame(gameLoop);
}

startGame();