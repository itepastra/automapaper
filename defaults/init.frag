#version 310 es
precision highp float;

uniform float time;
uniform vec2 resolution;

out vec4 stateColor;

float PHI = 1.61803398874989484820459;  // Φ = Golden Ratio   

float gold_noise(in vec2 xy, in float seed){
       return fract(tan(distance(xy*PHI, xy)*seed)*xy.x);
}

void main( void ) {

    vec2 position = gl_FragCoord.xy;
    float color = gold_noise(position.xy, fract(time));
    

    stateColor = vec4(step(0.3, color), 0,0,step(0.3, color));
}

