
// Receives data in aPosition, (x,y,z), returns a gl_position.
// Runs once per vertex.
export const vertexShaderSource = `#version 300 es
  in vec3 aPosition;

  void main() {
    gl_Position = vec4(aPosition, 1.0);
    gl_PointSize = 12.0; // Point size in pixels (for now boids are points)
  }
`;

// Returns a vector of red. 
// Runs for fragments generated from rasterizing triangles between vertices
export const fragmentShaderSource = `#version 300 es
  precision highp float;

  out vec4 fragColor;

  void main() {
    fragColor = vec4(1.0, 0.0, 0.0, 1.0);
  }
`;