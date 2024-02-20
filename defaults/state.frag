#version 100
precision highp float;

uniform sampler2D state;
uniform vec2 scale;

vec4 get(int x, int y) {
    return texture2D(state, (gl_FragCoord.xy + vec2(x, y)) / scale);
}

void main() {
    int sum = int(get(-1, -1).r +
              get(-1,  0).r +
              get(-1,  1).r +
              get( 0, -1).r +
              get( 0,  1).r +
              get( 1, -1).r +
              get( 1,  0).r +
              get( 1,  1).r);
    vec4 current = get(0,0);
    if (sum == 3) {
        gl_FragColor.r = 1.0;
        gl_FragColor.g = 1.0;
    } else if (sum == 2) {
        gl_FragColor = current;
        if (current.r == 0.0) {
            gl_FragColor.g = max(current.g - 0.01, 0.0);
        }
    } else {        
        gl_FragColor = vec4(0.0, max(current.g - 0.01, 0.0), 0.0, 1.0);
    }

}
