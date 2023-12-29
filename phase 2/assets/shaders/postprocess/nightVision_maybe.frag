#version 330

// The texture holding the scene pixels
uniform sampler2D tex;

// Read "assets/shaders/fullscreen.vert" to know what "tex_coord" holds;
in vec2 tex_coord;
out vec4 frag_color;


void main(){
    //Prep Scene Part
    vec4 initColor = texture(tex, tex_coord); //get color
    vec2 normTexCoord = tex_coord + vec2(-0.5,-0.5); // we want centre to be (0,0)
    normTexCoord = vec2(normTexCoord.x*1.5, normTexCoord.y); // 1.5 for aspect ratio not accurate tho :(
    float invlength = 1/(1+5*length(normTexCoord)); // inverse of length with a weight to achieve a steeper gradient
    initColor = initColor * invlength;
    vec4 nightVisionLight = vec4(invlength,invlength,invlength,0.4*invlength); // Simulate Night Vision Light, it is stronger in the middle
    initColor = nightVisionLight*nightVisionLight.a + initColor*(1-nightVisionLight.a); // Blend Night Vision with scene colors
    
    
    //Night Vision Part
    float bright = dot(initColor.rgb, vec3(0.3, 0.6, 0.1)); // get brightness
    bright = clamp(1.3 * (bright - 0.5) + 0.5 , 0.0, 1.0); // add contrast
    frag_color = vec4(0,bright,0,initColor.a);
    
}