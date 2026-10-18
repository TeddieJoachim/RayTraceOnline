// cube
const positions = new Float32Array([
  -1, -1, -1,  // 0
   1, -1, -1,  // 1
   1,  1, -1,  // 2
  -1,  1, -1,  // 3
  -1, -1,  1,  // 4
   1, -1,  1,  // 5
   1,  1,  1,  // 6
  -1,  1,  1   // 7
]);

const colors = new Float32Array([
  1,0,0,  0,1,0,  0,0,1, 1,1,0, 1,0,1, 0,1,1, 1,1,0, 1,0,1
]);


const indices = new Uint16Array([
  // Front
  4, 5, 6,   4, 6, 7,
  // Back
  1, 0, 3,   1, 3, 2,
  // Top
  3, 7, 6,   3, 6, 2,
  // Bottom
  0, 1, 5,   0, 5, 4,
  // Right
  1, 2, 6,   1, 6, 5,
  // Left
  0, 4, 7,   0, 7, 3,
]);


// SPHERE
function createSphere(radius, segments, rings) {
    const positions = [];
    const colors = [];
    const indices = [];

    for (let i = 0; i <= rings; i++) {

        const theta = i * Math.PI / rings;

        for (let j = 0; j <= segments; j++) {

            const phi = j * 2 * Math.PI / segments;

            const x = radius * Math.sin(theta) * Math.cos(phi);
            const y = radius * Math.cos(theta);
            const z = radius * Math.sin(theta) * Math.sin(phi);

            // all spheres believe themselves to be centered at the origin
            positions.push(x, y, z);

            colors.push(
                0.5 + 0.5 * x / radius,
                0.5 + 0.5 * y / radius,
                0.5 + 0.5 * z / radius
            );
        }
    }

    for (let i = 0; i < rings; i++) {

        for (let j = 0; j < segments; j++) {

            const first = i * (segments + 1) + j;
            const second = first + segments + 1;

            indices.push(
                first, second, first + 1,
                second, second + 1, first + 1
            );
        }
    }

    return {
        positions: new Float32Array(positions),
        colors: new Float32Array(colors),
        indices: new Uint16Array(indices)
    };
}