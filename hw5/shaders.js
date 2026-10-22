const fragmentShader = `#version 300 es
  precision mediump float;
  in vec3 vColor;

  out vec4 fragColor;

  void main() {
    fragColor = vec4(vColor,1.0);
  }`

const vertexShader = `#version 300 es
  in vec3 aPosition;
  in vec3 aColor;

  uniform float uTime; //time in sec
  uniform mat4 uModelViewMatrix;
  uniform mat4 uProjectionMatrix;
  uniform mat4 uModelTransformationMatrix;

  out vec3 vColor;

  void main() {
    gl_Position = uProjectionMatrix * uModelViewMatrix * uModelTransformationMatrix * vec4(aPosition,1.0);
    vColor = aColor;
  }`

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

    // for the rotations and uniform scaling of this sphere
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
  uniform float uSecondLightIntensity;

  out vec4 fragColor;

  
  void main() {
    vec3 N = normalize(vNormal);
    vec3 V = normalize(-vPosition);

    // original light
    vec3 L1 = normalize(vec3(-0.4, 0.6, 1.0));
    float diffuse1 = max(dot(N, L1), 0.0);

    vec3 H1 = normalize(L1 + V);
    float specular1 = 0.0;

    if (diffuse1 > 0.0) {
        specular1 = pow(max(dot(N, H1), 0.0), 64.0);
    }

    // user controlled second light
    vec3 L2 = normalize(vec3(0.6, 0.3, 1.0));
    float diffuse2 = max(dot(N, L2), 0.0);

    vec3 H2 = normalize(L2 + V);
    float specular2 = 0.0;

    if (diffuse2 > 0.0) {
        specular2 = pow(max(dot(N, H2), 0.0), 64.0);
    }

    // original lighting
    vec3 ambientColor = 0.15 * vColor;
    vec3 diffuseColor = 0.75 * diffuse1 * vColor;
    vec3 specularColor = specular1 * vec3(1.0);

    // second light adds broad illumination and a highlight
    vec3 glowColor = vec3(0.3, 0.8, 1.0);
    vec3 secondLight =
        uSecondLightIntensity *
        (0.65 * diffuse2 * vColor
          + 0.8 * specular2 * vec3(1.0)
          + 0.45 * glowColor * vColor);

    vec3 color =
        ambientColor + diffuseColor + specularColor + secondLight;

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
    // calculate the face normal using screen-space derivatives
    vec3 N = normalize(cross(dFdx(vPosition), dFdy(vPosition)));

    // direction of the first light source
    vec3 L = normalize(vec3(-0.4, 0.6, 1.0));

    // diffuse lighting
    float diffuse = max(dot(N, L), 0.0);

    // ambient and diffuse contributions
    vec3 ambientColor = 0.15 * vColor;
    vec3 diffuseColor = 0.85 * diffuse * vColor;

    // final color
    vec3 color = ambientColor + diffuseColor;
    fragColor = vec4(color, 1.0);
  }`;

function createShader(gl, type, source) {
  let shader = gl.createShader(type);
  gl.shaderSource(shader, source);
  gl.compileShader(shader);
  if (!gl.getShaderParameter(shader, gl.COMPILE_STATUS)) {
    throw new Error(gl.getShaderInfoLog(shader));
  }
  return shader;
}

function createProgram(gl, vsSource, fsSource) {
  let vs = createShader(gl, gl.VERTEX_SHADER, vsSource);
  let fs = createShader(gl, gl.FRAGMENT_SHADER, fsSource);
  let prog = gl.createProgram();
  gl.attachShader(prog, vs);
  gl.attachShader(prog, fs);
  gl.linkProgram(prog);
  if (!gl.getProgramParameter(prog, gl.LINK_STATUS)) {
    throw new Error(gl.getProgramInfoLog(prog));
  }
  return prog;
}

function initSpecularProgram(gl) {
  const program = createProgram(
    gl,
    specularVertexShader,
    specularFragmentShader
  );

  return {
    program,
    uSecondLightIntensity:gl.getUniformLocation(program, "uSecondLightIntensity"),
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