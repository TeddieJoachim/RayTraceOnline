let Module;

// async needed so that javascript doesn't just mosey along and do other shit
async function startGame() {
  Module = await createWASMModule(); // Locks things up until we get our WASM

  Module._initializeBoids(5);

  runGame(Module);
}


function runGame(Module) {
  //call C code to update bird positions in heap memory 
  let running = true;
  const boidsPointer = Module._initializeBoids();
  const numBoids = Module._getNumBoids(); // again we can ask or we can tell
  const boidSize = Module._howManyBytesInABoid();
  const sharedHeapCoords = new Float32Array(Module.HEAPU8.buffer, Module._getBoidPositions(), numBoids * 3); // a contigous array of Boids. is this best way to access?
  while (running) {
    Module._advanceBoids();

    //render
    running = false;
  }

  Module._freeBoids();
}

startGame();