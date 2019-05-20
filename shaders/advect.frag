#version 330 core

//Input Data
in vec2 coords;

// Output data
out vec4 targetOut;

uniform float timestep;
uniform float rdx;
uniform sampler2D velocity;
uniform sampler2D target;


void main() {
	//texelCoord refers to the center of the texel! Not a corner!
  
  vec2 pos = coords - timestep * rdx * texture2D(velocity, coords).xy;
  
  //find nearest bottom left corner
  vec2 corner = floor(coords);
  
  vec4 tex00 = texture2D(target, corner + vec2(0, 0));
  vec4 tex10 = texture2D(target, corner + vec2(1, 0));
  vec4 tex01 = texture2D(target, corner + vec2(0, 1));
  vec4 tex11 = texture2D(target, corner + vec2(1, 1));

  //bilinear interpolation
  targetOut = mix(mix(tex00, tex10, pos.x), mix(tex01, tex11, pos.x), pos.y);

}