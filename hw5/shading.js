
const specularVertexShader = `#version 300 es
in vec3 aPosition;
in vec3 aColor;

uniform mat4 uModelViewMatrix;
uniform mat4 uProjectionMatrix;
uniform mat4 uModelTransformationMatrix;

out vec3 vColor;
out vec3 vNormal;
out vec3 vPosition;

void main() {
    vec4 viewPosition =
        uModelViewMatrix *
        uModelTransformationMatrix *
        vec4(aPosition, 1.0);

    gl_Position = uProjectionMatrix * viewPosition;

    // Valid for the rotations and uniform scaling of this sphere.
    vNormal = normalize(
        mat3(uModelViewMatrix * uModelTransformationMatrix)
        * normalize(aPosition)
    );

    vPosition = viewPosition.xyz;
    vColor = aColor;
}`;

const specularFragmentShader = `#version 300 es
precision highp float;

in vec3 vColor;
in vec3 vNormal;
in vec3 vPosition;

out vec4 fragColor;

void main() {
    vec3 N = normalize(vNormal);

    // Light direction in view coordinates.
    vec3 L = normalize(vec3(-0.4, 0.6, 1.0));

    // The camera is at the origin in view coordinates.
    vec3 V = normalize(-vPosition);

    // Diffuse lighting.
    float diffuse = max(dot(N, L), 0.0);

    // Blinn-Phong specular highlight.
    vec3 H = normalize(L + V);
    float specular = 0.0;

    if (diffuse > 0.0) {
        specular = pow(max(dot(N, H), 0.0), 64.0);
    }

    vec3 ambientColor = 0.15 * vColor;
    vec3 diffuseColor = 0.75 * diffuse * vColor;
    vec3 specularColor = 1.0 * specular * vec3(1.0);

    vec3 color = ambientColor + diffuseColor + specularColor;

    fragColor = vec4(color, 1.0);
}`;

const flatVertexShader = `#version 300 es
in vec3 aPosition;
in vec3 aColor;

uniform mat4 uModelViewMatrix;
uniform mat4 uProjectionMatrix;
uniform mat4 uModelTransformationMatrix;

out vec3 vColor;
out vec3 vPosition;

void main() {
    vec4 viewPosition =
        uModelViewMatrix *
        uModelTransformationMatrix *
        vec4(aPosition, 1.0);

    gl_Position = uProjectionMatrix * viewPosition;

    vPosition = viewPosition.xyz;
    vColor = aColor;
}`;

const flatFragmentShader = `#version 300 es
precision highp float;

in vec3 vColor;
in vec3 vPosition;

out vec4 fragColor;

void main() {
    // Calculate one face normal per triangle.
    vec3 N = normalize(cross(dFdx(vPosition), dFdy(vPosition)));

    // Light direction in view space.
    vec3 L = normalize(vec3(-0.4, 0.6, 1.0));

    // Diffuse lighting.
    float diffuse = max(dot(N, L), 0.0);

    // Ambient + diffuse contribution.
    vec3 color = vColor * (0.15 + 0.85 * diffuse);

    fragColor = vec4(color, 1.0);
}`;

function initSpecularProgram(gl) {
    const program = createProgram(
        gl,
        specularVertexShader,
        specularFragmentShader
    );

    return {
        program,
        posLoc: gl.getAttribLocation(program, "aPosition"),
        colorLoc: gl.getAttribLocation(program, "aColor"),
        uMVM: gl.getUniformLocation(program, "uModelViewMatrix"),
        uPM: gl.getUniformLocation(program, "uProjectionMatrix"),
        uMTM: gl.getUniformLocation(program, "uModelTransformationMatrix")
    };
}

function initFlatProgram(gl) {
    const program = createProgram(
        gl,
        flatVertexShader,
        flatFragmentShader
    );

    return {
        program,
        posLoc: gl.getAttribLocation(program, "aPosition"),
        colorLoc: gl.getAttribLocation(program, "aColor"),
        uMVM: gl.getUniformLocation(program, "uModelViewMatrix"),
        uPM: gl.getUniformLocation(program, "uProjectionMatrix"),
        uMTM: gl.getUniformLocation(program, "uModelTransformationMatrix")
    };
}
