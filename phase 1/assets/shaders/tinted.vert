#version 330 core

layout(location = 0) in vec3 position;
layout(location = 1) in vec4 color;

uniform mat4 object_to_world;
uniform mat4 view_projection;

out Varyings {
    vec4 color;
} vs_out;

uniform mat4 transform;

void main(){
    //TODO: (Req 7) Change the next line to apply the transformation matrix
   // gl_Position = vec4(position, 1.0);
     gl_Position = transform * vec4(position, 1.0);
    vs_out.color = color;
}