#pragma once

#include "../ecs/world.hpp"
#include "../components/camera.hpp"
#include "../components/light.hpp"
#include "../components/free-camera-controller.hpp"

#include "../application.hpp"

#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>
#include <glm/trigonometric.hpp>
#include <glm/gtx/fast_trigonometry.hpp>

namespace our
{
    class WinSystem {
        Application* app;
        CameraComponent* camera = nullptr;
        LightComponent* pointLight = nullptr;
        Entity* player = nullptr;
        Entity* winBarrier = nullptr;
        Entity* winParent = nullptr;
        const float winObjectSpeed = 1.0f;
        const float cameraAngular = 0.3f;
        const float lightDecay = 0.7f;
        const float lightThreshold = 0.005f;
        const float timeOfMaxLight = 10.0f;
        float timeAccum = 0;
        const glm::vec3 specular = glm::vec3(0, 1, 1);
        const glm::vec3 diffuse = glm::vec3(1, 0, 0);
        const glm::vec3 ambient = glm::vec3(1, 1, 1);
        float accum = 1.0f;
        std::vector<LightComponent*> directionLights;
    public:
        void enter(Application* app){
            this->app = app;
        }

        void update(World* world, float deltaTime) {
            if (!(camera && player && winBarrier && winParent)){
                for(auto entity : world->getEntities()){
                    if (!player && entity->name == "player") player = entity;
                    if (!camera) camera = entity->getComponent<CameraComponent>();
                    if (entity->name == "win-barrier") winBarrier = entity;
                    if (entity->name == "win-parent") winParent = entity;
                    LightComponent* tempLight = entity->getComponent<LightComponent>();
                    if (!pointLight && tempLight && tempLight->lightType == LightType::POINT) pointLight = tempLight;
                    if (tempLight && tempLight->lightType == LightType::DIRECTIONAL) directionLights.push_back(tempLight);
                }
            }

            if(!(camera && player && winBarrier && winParent)) return;

            if (accum > lightThreshold){
                for (auto &light: directionLights){
                    const float factor = glm::pow(lightDecay, deltaTime);
                    accum *= factor;
                    glm::vec3 vecFactor = glm::vec3(factor);
                    light->diffuse *= vecFactor;
                    light->specular *= vecFactor;
                    light->ambient *= vecFactor;
                }
            }

            if (pointLight){
                timeAccum += deltaTime;
                const float interpolation = glm::smoothstep(0.0f, timeOfMaxLight, timeAccum);
                pointLight->diffuse = interpolation * diffuse;
                pointLight->specular = interpolation * specular;
                pointLight->diffuse = interpolation * diffuse;
            }

            winBarrier->localTransform.position.y = 0;

            glm::vec3& position = winParent->localTransform.position;
            position.y += winObjectSpeed * deltaTime;
            position.y = glm::min(position.y, 1.0f);


            glm::vec3& rotation = camera->getOwner()->localTransform.rotation;
            rotation.x += cameraAngular * deltaTime;
            rotation.x = glm::min(rotation.x, -5.0f / 180.0f * glm::pi<float>());
        }

        void exit(){
            timeAccum = 0;
            accum = 1;
            pointLight = nullptr;
            camera = nullptr;
            player = nullptr;
            winBarrier = nullptr;
            winParent = nullptr;
            directionLights.clear();
        }
    };
}
