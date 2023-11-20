#version 330 core

out vec4 frag_color;

// In this shader, we want to draw a checkboard where the size of each tile is (size x size).
// The color of the top-left most tile should be "colors[0]" and the 2 tiles adjacent to it
// should have the color "colors[1]".

//TODO: (Req 1) Finish this shader.

uniform int size = 32;
uniform vec3 colors[2];

bool checkerboard_check(vec4 coord, int size){
    uint x_index = uint(coord.x / float(size));
    uint y_index = uint(coord.y / float(size));

    return (mod((x_index + y_index), 2) == 0);
}

void main(){
    //a - (b * floor(a/b))
    if(checkerboard_check(gl_FragCoord, size))
        frag_color = vec4(colors[0], 1.0);
    else
        frag_color = vec4(colors[1], 1.0);
}