#version 330 core

out vec4 frag_color;

// In this shader, we want to draw a checkboard where the size of each tile is (size x size).
// The color of the top-left most tile should be "colors[0]" and the 2 tiles adjacent to it
// should have the color "colors[1]".

//DONE: (Req 1) Finish this shader.

uniform int size = 32;
uniform vec3 colors[2];

void main(){
    int xTile = int(gl_FragCoord.x/size);  //Divide by size so x increases by 1 only when x coord increases by size
    int yTile = int(gl_FragCoord.y/size);  //Divide by size so y increases by 1 only when y coord increases by size
    frag_color = vec4(colors[(xTile+yTile)%2], 1.0); // I check the sum of x and y coords even => 0 odd => 1
                                             // (0,0) => 0 (0,1) => 1 (1,0) => 1 (1,1) => 0 .....etc
}