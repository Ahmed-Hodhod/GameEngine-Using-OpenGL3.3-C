#include "material.hpp"

#include "../asset-loader.hpp"
#include "deserialize-utils.hpp"

#define TEXTURE_UINT_0 0

namespace our {

    // This function should setup the pipeline state and set the shader to be used
    void Material::setup() const {
        //DONE: (Req 7) Write this function
        pipelineState.setup();
        shader->use();
        //Task1
        //In contrast to the lit material class, these single values still allow us to have materials that respond
        // to light even if we don't have albedo, specular, roughness, ambient occlusion and emissive textueres
        //Before each draw call, these material properties are sent to the shader        
        shader->set("material.diffuse", diffuse);
        shader->set("material.specular", specular);
        shader->set("material.ambient", ambient);
        shader->set("material.shininess", shininess);
    }

    // This function read the material data from a json object
    void Material::deserialize(const nlohmann::json& data){
        if(!data.is_object()) return;

        if(data.contains("pipelineState")){
            pipelineState.deserialize(data["pipelineState"]);
        }
        shader = AssetLoader<ShaderProgram>::get(data["shader"].get<std::string>());
        transparent = data.value("transparent", false);        
        
        //Task1 added deserialization for these new material attributes
        diffuse = data.value("diffuse",  diffuse);
        specular = data.value("specular", specular);
        ambient = data.value("ambient", ambient);

        shininess = data.value("shininess", shininess);
    }

    // This function should call the setup of its parent and
    // set the "tint" uniform to the value in the member variable tint 
    void TintedMaterial::setup() const {
        //DONE: (Req 7) Write this function
        Material::setup();
        shader->set("tint", tint);
    }

    // This function read the material data from a json object
    void TintedMaterial::deserialize(const nlohmann::json& data){
        Material::deserialize(data);
        if(!data.is_object()) return;
        tint = data.value("tint", glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));
    }

    // This function should call the setup of its parent and
    // set the "alphaThreshold" uniform to the value in the member variable alphaThreshold
    // Then it should bind the texture and sampler to a texture unit and send the unit number to the uniform variable "tex" 
    void TexturedMaterial::setup() const {
        //DONE: (Req 7) Write this function
        TintedMaterial::setup();
        shader->set("alphaThreshold", alphaThreshold);
        glActiveTexture(GL_TEXTURE0);
        texture->bind();
        if (sampler) sampler->bind(TEXTURE_UINT_0);
        shader->set("tex", TEXTURE_UINT_0);
    }

    // This function read the material data from a json object
    void TexturedMaterial::deserialize(const nlohmann::json& data){
        TintedMaterial::deserialize(data);
        if(!data.is_object()) return;
        alphaThreshold = data.value("alphaThreshold", 0.0f);
        texture = AssetLoader<Texture2D>::get(data.value("texture", ""));
        sampler = AssetLoader<Sampler>::get(data.value("sampler", ""));
    }

    void LitMaterial::setup() const {
        pipelineState.setup();
        shader->use();

        //Send all the material tint values to the texture
        shader->set("tex_mat.albedoTint", albedoTint);
        shader->set("tex_mat.specularTint", specularTint);
        shader->set("tex_mat.roughnessRange", roughnessRange);
        shader->set("tex_mat.emissiveTint", emissiveTint);

        shader->set("alphaThreshold", alphaThreshold);
        //Task2
        //If a texture was specified in the json file, then it should be bound to a texture unit
        //Otherwise we set the the uniform to sample from an texture unit (texture unit 5) which has a 1 pixel black texture bound
        // so when sampled from, it returns the color black

        if(albedo){ 
            glActiveTexture(GL_TEXTURE0);
            albedo->bind();
            shader->set("tex_mat.albedo", 0);
        } else shader->set("tex_mat.albedo", 5);
        
        if(specular){
            glActiveTexture(GL_TEXTURE1);
            specular->bind();
            shader->set("tex_mat.specular", 1);
        } else shader->set("tex_mat.specular", 5);
        
        if(roughness){
            glActiveTexture(GL_TEXTURE2);
            roughness->bind();
            shader->set("tex_mat.roughness", 2);
        } else shader->set("tex_mat.roughness", 5);
        
        if(ambientOcclusion){ 
            glActiveTexture(GL_TEXTURE3);
            ambientOcclusion->bind();
            shader->set("tex_mat.ambientOcclusion", 3);
        } else shader->set("tex_mat.ambientOcclusion", 5);
        
        if(emissive){ 
            glActiveTexture(GL_TEXTURE4);
            emissive->bind();
            shader->set("tex_mat.emissive", 4);
        } else shader->set("tex_mat.emissive", 5);

        glActiveTexture(GL_TEXTURE5);
        AssetLoader<Texture2D>::get("black")->bind();
        
        //We have a single sampler which contains all the sampling options (such as MIN_FILTER: LINEAR) and is 
        // bound to all the texture units being used by the class
        if (sampler){ 
            sampler->bind(0);
            sampler->bind(1);
            sampler->bind(2);
            sampler->bind(3);
            sampler->bind(4);
        }
    }

    // This function read the material data from a json object
    void LitMaterial::deserialize(const nlohmann::json& data){
        if(!data.is_object()) return;

        if(data.contains("pipelineState")){
            pipelineState.deserialize(data["pipelineState"]);
        }
        shader = AssetLoader<ShaderProgram>::get(data["shader"].get<std::string>());
        transparent = data.value("transparent", false);     
        
        //Task2
        //added deserialization for these new material attributes
        albedoTint = data.value("albedoTint", glm::vec3(1.0f, 1.0f, 1.0f));
        specularTint = data.value("specularTint", glm::vec3(1.0f, 1.0f, 1.0f));
        roughnessRange = data.value("roughnessRange", glm::vec2(0.0f, 1.0f));
        emissiveTint = data.value("emissiveTint", glm::vec3(1.0f, 1.0f, 1.0f));

        alphaThreshold = data.value("alphaThreshold", 0.0f);

        albedo = AssetLoader<Texture2D>::get(data.value("albedo", ""));
        specular = AssetLoader<Texture2D>::get(data.value("specular", ""));
        roughness = AssetLoader<Texture2D>::get(data.value("roughness", ""));
        ambientOcclusion = AssetLoader<Texture2D>::get(data.value("ambientOcclusion", ""));
        emissive = AssetLoader<Texture2D>::get(data.value("emissive", ""));

        sampler = AssetLoader<Sampler>::get(data.value("sampler", ""));
    }

}