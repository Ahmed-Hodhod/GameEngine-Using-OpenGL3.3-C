#version 330 core

in Varyings {
    vec4 color;
    vec2 tex_coord;
    vec3 world;
    vec3 view;
    vec3 normal;
} fsin;

#define TYPE_DIRECTIONAL 0
#define TYPE_POINT 1
#define TYPE_SPOT 2

struct Light {
    int type;
    vec3 diffuse;
    vec3 specular;
    vec3 ambient;
    vec3 position, direction;
    vec3 attenuation;
    vec2 cone;
};

struct Material {
    vec3 diffuse;
    vec3 specular;
    vec3 ambient;
    float shininess;
};

uniform Material material;
#define MAX_LIGHT_COUNT 69
uniform Light lights[MAX_LIGHT_COUNT];
uniform int light_count;
out vec4 frag_color;

uniform vec4 tint;
uniform sampler2D tex;

void main() {
    vec3 normal = normalize(fsin.normal);
    vec3 view = normalize(fsin.view);
    int count = min(light_count, MAX_LIGHT_COUNT);
    vec3 accumulated_light = vec3(0.0);

    for(int index = 0; index < count; index++){
        Light light = lights[index];
        vec3 light_direction;
        float attenuation = 1;
        if(light.type == TYPE_DIRECTIONAL)
            light_direction = light.direction;
        else {
            light_direction = fsin.world - light.position;
            float distance = length(light_direction);
            light_direction /= distance;
            attenuation *= 1.0f / dot(light.attenuation, vec3(distance * distance, distance, 1));
            if(light.type == TYPE_SPOT){
                float angle = acos(dot(light.direction, light_direction));
                attenuation *= smoothstep(light.cone.x, light.cone.y, angle);
            }
        }

        vec3 reflected = reflect(light_direction, normal);
        float lambert = max(0.0f, dot(normal, -light_direction));
        float phong = pow(max(0.0f, dot(view, reflected)), material.shininess);

        vec3 diffuse = material.diffuse * light.diffuse * lambert;
        vec3 specular = material.specular * light.specular * phong;
        vec3 ambient = material.ambient * light.ambient;
        
        accumulated_light += (diffuse + specular + ambient) * attenuation;
    }

    frag_color = tint * fsin.color * texture(tex, fsin.tex_coord) * vec4(accumulated_light, 1.0f);
}