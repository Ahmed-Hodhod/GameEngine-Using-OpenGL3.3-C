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
    //diffuse, specular and ambient are properties that are used by all light types
    vec3 diffuse;
    vec3 specular;
    vec3 ambient;
    //position and attenutation is useful for point and spot lights while direction is useful only for the latter
    vec3 position, direction;
    //(c2, c1, c0) which are factors for d^2, d, 1 where d is the distance from the camera to the point
    vec3 attenuation;
    //(outer_angle, inner_angle)
    //Light is at full intensity within inner_angle, is attenuated when between the two angles
    // and is 0 outside outer_angle
    vec2 cone;
};

//material properties which are sampled from albedo, specular, roughness, ambient occlusion and emissive textures
struct Material {
    vec3 diffuse;   //samples dirctly from albedo
    vec3 specular;  //samples dirctly from specular
    vec3 ambient;   //samplers from ambient occlusion tinted with the diffuse
    vec3 emissive;  //samples directly from emissive
    float shininess;//roughness is sampled from the texture and mapped to shininess
    float transparency;    //used for transparency
};

//structure to hold the texture unit numbers which the relevant texture is bound to
//it also contains tints
struct TexturedMaterial {
    sampler2D albedo;
    vec3 albedoTint;
    sampler2D specular;
    vec3 specularTint;
    sampler2D ambientOcclusion;
    sampler2D roughness;
    vec2 roughnessRange;
    sampler2D emissive;
    vec3 emissiveTint;
};

Material sample_material(TexturedMaterial tex_mat, vec2 tex_coord){
    Material mat;

    vec4 albedo = texture(tex_mat.albedo, tex_coord);
    mat.diffuse = tex_mat.albedoTint * albedo.rgb;
    mat.transparency = albedo.a;

    mat.specular = tex_mat.specularTint * texture(tex_mat.specular, tex_coord).rgb;
    mat.emissive = tex_mat.emissiveTint * texture(tex_mat.emissive, tex_coord).rgb;
    mat.ambient = mat.diffuse * texture(tex_mat.ambientOcclusion, tex_coord).r;

    //here roughness is inerpolating between roughness_min, and roughness_+max
    float roughness = mix(tex_mat.roughnessRange.x, tex_mat.roughnessRange.y, texture(tex_mat.roughness, tex_coord).r);
    //mapping clamped roughness to shininess, this is done so that specular is never infinity and never 0 
    mat.shininess = 2.0f / pow(clamp(roughness, 0.001f, 0.999f), 4.0f) - 2.0f;

    return mat;
}

//uniform containing all the samplers
uniform TexturedMaterial tex_mat;
#define MAX_LIGHT_COUNT 69
uniform Light lights[MAX_LIGHT_COUNT];
uniform int light_count;
out vec4 frag_color;

void main() {
    vec3 normal = normalize(fsin.normal);
    vec3 view = normalize(fsin.view);
    int count = min(light_count, MAX_LIGHT_COUNT);
    vec3 accumulated_light = vec3(0.0);
    //this is where we sample the material properties from their textures
    Material material = sample_material(tex_mat, fsin.tex_coord);

    //for each light in the scene, we compute it's effect on the pixel
    for(int index = 0; index < count; index++){
        Light light = lights[index];
        vec3 light_direction;
        float attenuation = 1;
        if(light.type == TYPE_DIRECTIONAL)
            //here the direction of the light is simply the dirction of the directional light
            light_direction = light.direction;
        else {
            //but for spot and point lights, the light is emitting radially outwards from the light position
            light_direction = fsin.world - light.position;
            float distance = length(light_direction);
            //normalizing the light vector so that the angle can be computed later without having to worry about vector length
            light_direction /= distance;
            //attenuate based on how far the point is from the light and the attenuation factors of the light
            attenuation *= 1.0f / dot(light.attenuation, vec3(distance * distance, distance, 1));
            if(light.type == TYPE_SPOT){
                //compute the angle between the direction of the cone center "light.direction" and the 
                // direction from the light to the point of interest "light_direction"
                float angle = acos(dot(light.direction, light_direction));
                //attentuate further based on how far from the cone center the point is
                attenuation *= smoothstep(light.cone.x, light.cone.y, angle);
            }
        }
        //compute the reflected light vector
        vec3 reflected = reflect(light_direction, normal);
        //negative is taken as normal is pointing away from the object but light is pointing towards
        float lambert = max(0.0f, dot(normal, -light_direction));
        float phong = pow(max(0.0f, dot(view, reflected)), material.shininess);

        vec3 diffuse = material.diffuse * light.diffuse * lambert;
        vec3 specular = material.specular * light.specular * phong;
        vec3 ambient = material.ambient * light.ambient;
        //add the effect of this light to the effects of all prior lights and account for attenuation
        accumulated_light += (diffuse + specular + ambient) * attenuation;
    }
    //use the albedo's alpha for transperancy
    frag_color = fsin.color * vec4(accumulated_light + material.emissive, material.transparency);
}