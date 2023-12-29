#pragma once

#include "../ecs/world.hpp"
#include "../components/camera.hpp"
#include "../components/free-camera-controller.hpp"

#include "../application.hpp"

#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>
#include <glm/trigonometric.hpp>
#include <glm/gtx/fast_trigonometry.hpp>

namespace our
{

    // The free camera controller system is responsible for moving every entity which contains a FreeCameraControllerComponent.
    // This system is added as a slightly complex example for how use the ECS framework to implement logic. 
    // For more information, see "common/components/free-camera-controller.hpp"
    class CameraLockSystem {
        Application* app; // The application in which the state runs
        CameraComponent* camera = nullptr;
        Entity* player = nullptr;
    public:
        // When a state enters, it should call this function and give it the pointer to the application
        void enter(Application* app){
            this->app = app;
        }

        // This should be called every frame to update all entities containing a FreeCameraControllerComponent 
        void update(World* world, float deltaTime) {
            if (!(camera && player)){
                for(auto entity : world->getEntities()){
                    if (!player && entity->name == "player") player = entity;
                    if (!camera){
                        camera = entity->getComponent<CameraComponent>();
                    }
                    if(camera && player) break;
                }
            }
            // If there is no entity with both a CameraComponent and a ChickenCameraControllerComponent, we can do nothing so we return
            if(!(camera && player)) return;

            // We get a reference to the entity's position
            glm::vec3& position = camera->getOwner()->localTransform.position;

            position = glm::vec3(0, 4, player->localTransform.position.z + 2.0);   
        }

        void exit(){
            camera = nullptr;
            player = nullptr;
        }
    };
}
