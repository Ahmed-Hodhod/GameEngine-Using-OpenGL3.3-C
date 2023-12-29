#pragma once

#include "../ecs/world.hpp"
#include "../application.hpp"

#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>
#include <glm/gtx/euler_angles.hpp>

namespace our
{

    class CarMovementSystem {
        Application* app; // The application in which the state runs
        const float road_width = 24.0f;
        const float car_length = 1.5f;
        glm::vec3 v = glm::vec3(4.0f, 0.0f, 0.0f);
        std::vector<Entity*> cars;
        Entity* player = nullptr;
    public:
        // When a state enters, it should call this function and give it the pointer to the application
        void enter(Application* app){
            this->app = app;
        }

        // This should be called every frame to update all entities containing a FreeCameraControllerComponent 
        void update(World* world, float deltaTime) {
            if (!player){
                for(auto entity : world->getEntities()){
                    if (!player && entity->name == "player") player = entity;
                }
            }
            if(!player) return;
            if (cars.empty()){
                for(auto entity : world->getEntities()){
                    if (entity->name == "car") cars.push_back(entity);
                }
                std::sort(cars.begin(), cars.end(), [](Entity*& first, Entity*& second){
                    return first->localTransform.position.z > second->localTransform.position.z;
                });
            }
            float lmao = 1;
            for (auto& car: cars){
                Transform& transform = car->localTransform;
                if (-lmao * transform.position.x > road_width/2.0f) {
                    transform.position += glm::vec3(lmao) * glm::vec3(road_width - car_length, 0, 0);
                }
                else {
                    glm::vec3 carCenter = car->getLocalToWorldMatrix() * glm::vec4(0, 0, 0, 1);
                    glm::vec3 playerCenter = player->getLocalToWorldMatrix() * glm::vec4(0, 0, 0, 1);
                    float distance = glm::distance(carCenter, playerCenter);
                    if (distance > 1.3)   
                        transform.position -= glm::vec3(lmao) * v * deltaTime;
                }
                lmao = -lmao;
            }
        }

        void exit(){
            cars.clear();
            player = nullptr;
        }
    };

}
