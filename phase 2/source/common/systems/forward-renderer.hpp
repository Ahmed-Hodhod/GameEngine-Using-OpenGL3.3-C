#pragma once

#include "../ecs/world.hpp"
#include "../components/camera.hpp"
#include "../components/light.hpp"
#include "../components/mesh-renderer.hpp"
#include "../asset-loader.hpp"

#include <glad/gl.h>
#include <vector>
#include <algorithm>

namespace our
{
    
    // The render command stores command that tells the renderer that it should draw
    // the given mesh at the given localToWorld matrix using the given material
    // The renderer will fill this struct using the mesh renderer components
    struct RenderCommand {
        glm::mat4 localToWorld;
        glm::vec3 center;
        Mesh* mesh;
        Material* material;
    };
    //Task4
    struct LightEffect {
        LightType lightType;
        glm::vec3 diffuse;
        glm::vec3 specular;
        glm::vec3 ambient;
        glm::vec3 position, direction;
        glm::vec3 attenuation;
        glm::vec2 cone;
        LightEffect(LightComponent* light){
            lightType = light->lightType;
            direction = light->getDirection();
            diffuse = light->diffuse;
            specular = light->specular;
            ambient = light->ambient;
            position = light->getPosition();
            attenuation = light->attenuation;
            cone = light->cone;
        }

        void static setup(RenderCommand& command, std::vector<LightEffect>& lightEffects){
            int index = 0;
            for (auto &lightEffect : lightEffects){
                //A switch case is too much work
                std::string name = "lights[" + std::to_string(index++) + "].";
                command.material->shader->set(name + "type", (int)lightEffect.lightType);
                command.material->shader->set(name + "diffuse", lightEffect.diffuse);
                command.material->shader->set(name + "specular", lightEffect.specular);
                command.material->shader->set(name + "ambient", lightEffect.ambient);
                command.material->shader->set(name + "position", lightEffect.position);
                command.material->shader->set(name + "direction", lightEffect.direction);
                command.material->shader->set(name + "attenuation", lightEffect.attenuation);
                command.material->shader->set(name + "cone", lightEffect.cone);
            }
            command.material->shader->set("light_count", (int)lightEffects.size());
        }
    };

    // A forward renderer is a renderer that draw the object final color directly to the framebuffer
    // In other words, the fragment shader in the material should output the color that we should see on the screen
    // This is different from more complex renderers that could draw intermediate data to a framebuffer before computing the final color
    // In this project, we only need to implement a forward renderer
    class ForwardRenderer {
        // These window size will be used on multiple occasions (setting the viewport, computing the aspect ratio, etc.)
        glm::ivec2 windowSize;
        // These are two vectors in which we will store the opaque and the transparent commands.
        // We define them here (instead of being local to the "render" function) as an optimization to prevent reallocating them every frame
        std::vector<RenderCommand> opaqueCommands;
        std::vector<RenderCommand> transparentCommands;
        //Task4
        std::vector<LightEffect> lightEffects;
        // Objects used for rendering a skybox
        Mesh* skySphere;
        TexturedMaterial* skyMaterial;
        // Objects used for Postprocessing
        GLuint postprocessFrameBuffer, postProcessVertexArray;
        Texture2D *colorTarget, *depthTarget;
        TexturedMaterial* postprocessMaterial;
        bool postprocessingRequested = false;
    public:
        // Initialize the renderer including the sky and the Postprocessing objects.
        // windowSize is the width & height of the window (in pixels).
        void initialize(glm::ivec2 windowSize, const nlohmann::json& config);
        // Clean up the renderer
        void destroy();
        // This function should be called every frame to draw the given world
        void render(World* world);
        void requestPostProcessing();


    };

}