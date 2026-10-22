const tentacle = [];
const numSpheres = 5;

// adding spheres to tentacle
for (let i = 0; i < numSpheres; i++) {
  const radius = Math.pow(0.85, i);

  tentacle.push({
    sphere: createSphere(radius, 32, 16),
    radius: radius,
    children: []
  });
}

// assigning children to tentacle nodes
for (let i = 0; i < numSpheres - 1; i++) {
  tentacle[i].children.push(tentacle[i + 1]);
}


const matrixStack = [];
function pushMatrix(matrix) {
  matrixStack.push(matrix);
}
function popMatrix() {
  return matrixStack.pop();
}

// animation variables
let jumpOffset = 0;
let jumping = false;
let jumpStartTime = 0;
// public function to begin animation
function startJump() {
  if (jumping) return;

  jumping = true;
  jumpStartTime = performance.now();

  requestAnimationFrame(updateJump);
}

function updateJump(time) {
  const elapsed = time - jumpStartTime;
  const duration = 1000; // 1 second

  if (elapsed >= duration) {
    jumpOffset = 0;
    jumping = false;
    render();
    return;
  }

  const t = elapsed / duration;

  // starts at 0, rises, then returns to 0
  jumpOffset = 2.0 * 4 * t * (1 - t);

  render();

  requestAnimationFrame(updateJump);
}

let foldAngle = 0;
let targetFoldAngle = 0;

function foldRight() {
  targetFoldAngle = -1.0;
}

function foldLeft() {
  targetFoldAngle = 1.0;
}

function unfold() {
  targetFoldAngle = 0;
}

// recursively draws the hierarchy
function drawNode(node, parentMatrix, yOffset, depth) {
  let currentMatrix = parentMatrix;

  // Move to this joint
  currentMatrix = mat4Translate(
    currentMatrix,
    [0, yOffset, 0]
  );

  // Rotate this joint
  currentMatrix = mat4RotateZ(
    currentMatrix,
    foldAngle * 0.5
  );

  pushMatrix(currentMatrix);

  drawSphere(node.sphere, currentMatrix);

  for (let child of node.children) {
    drawNode(
      child,
      currentMatrix,
      node.radius * 1.8,
      depth + 1
    );
  }

  popMatrix();
}


// function for rendering the whole hierarchy
function drawTentacle() {
  let rootMatrix = mat4Identity();

  // Move the entire tentacle upward
  rootMatrix = mat4Translate(
    rootMatrix,
    [0, jumpOffset, 0]
  );

  drawNode(tentacle[0], rootMatrix, 0, 0);
}