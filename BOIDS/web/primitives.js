
// WebGL coordinates range -1 to 1.
// This makes three vertices, defining a triangle.
export function createTriangle() {
  return new Float32Array([
    0.0, 0.5, 0.0,
    -0.5, -0.5, 0.0,
    0.5, -0.5, 0.0
  ])
}