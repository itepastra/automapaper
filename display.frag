#version 100
precision highp float;

uniform sampler2D tex2D;

varying highp vec2 texCoords;

void main() {
    vec4 state = texture2D(tex2D, texCoords);
    
    gl_FragColor = state * vec4(0.1, 0.1, 0.1, 1.0);
}
