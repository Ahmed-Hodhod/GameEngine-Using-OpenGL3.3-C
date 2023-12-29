#pragma once

#include "../ecs/world.hpp"

#include "../application.hpp"

#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>
#include <glm/gtx/euler_angles.hpp>

namespace our
{
    struct lock{
        bool 
        forward = false,
        left = false,
        right = false,
        backward = false;
    };
    class PlayerMovementSystem {
    public:
        Application* app; // The application in which the state runs
        Entity* player = nullptr;
        std::unordered_map<Entity*, bool> collisions;
        std::unordered_map<Entity*, lock> entityLocking;
        float facing = 180;
        const float deathAngular = 5;
        const float speed = 10;
        const glm::vec4 velocity = glm::vec4(0, 0, 1, 0);
        bool playing = true;
        bool 
        forward = true,
        left = true,
        right = true,
        backward = true;
        bool win = false;
   
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

            //print win
            //restart game
            if(!playing && app->getKeyboard().justPressed(GLFW_KEY_ENTER)){
                player->localTransform.position = glm::vec3(0, 1, 5);
                player->localTransform.rotation = glm::vec3(0, glm::pi<float>(), 0);
                forward = true;
                left = true;
                right = true;
                backward = true;
                playing = true;
                win = false;
                collisions.clear();
                entityLocking.clear();
            }
            
            if (!playing){
                if (left){
                    if (player->localTransform.rotation.z > -glm::pi<float>() / 2.0f) player->localTransform.rotation.z -= deathAngular * deltaTime;
                    if(player->localTransform.position.x > -5) player->localTransform.position.x -= speed * deltaTime;
                }else if (right){
                    if (player->localTransform.rotation.z < glm::pi<float>() / 2.0f) player->localTransform.rotation.z += deathAngular * deltaTime;
                    if(player->localTransform.position.x < 5) player->localTransform.position.x += speed * deltaTime;
                }else if (forward){
                    if (player->localTransform.rotation.x > -glm::pi<float>() / 2.0f) player->localTransform.rotation.x -= deathAngular * deltaTime;
                    if(player->localTransform.position.z > -30) player->localTransform.position.z -= speed * deltaTime;
                }else{
                    if (player->localTransform.rotation.x < glm::pi<float>() / 2.0f) player->localTransform.rotation.x += deathAngular * deltaTime;
                    if(player->localTransform.position.z < 20) player->localTransform.position.z += speed * deltaTime;
                }
                return;
            }
            glm::vec3 playerCenter;
            for(auto entity : world->getEntities()){
                if (entity->name == "floor" || entity->name == "light" || entity == player) continue;
                glm::vec3 entityCenter = entity->getLocalToWorldMatrix() * glm::vec4(0, 0, 0, 1);
                if (entityCenter.y >= 3 || entityCenter.y <= -3) continue;
                playerCenter = player->getLocalToWorldMatrix() * glm::vec4(0, 0, 0, 1);
                float distance = glm::distance(entityCenter, playerCenter);
                if (distance < 1.3){
                    onCollisionEnter(entity, entityCenter, playerCenter);       
                }else if (collisions.find(entity) != collisions.end() && collisions[entity]){
                    onCollisionExit(entity, entityCenter, playerCenter);
                }
            }

            if(app->getKeyboard().isPressed(GLFW_KEY_W) && forward) facing = glm::pi<float>();
            else if(app->getKeyboard().isPressed(GLFW_KEY_S) && backward) facing = 0.0f;
            else if(app->getKeyboard().isPressed(GLFW_KEY_D) && right) facing = glm::pi<float>() / 2.0f;
            else if(app->getKeyboard().isPressed(GLFW_KEY_A) && left) facing = -glm::pi<float>() / 2.0f;
            else return;
            player->localTransform.rotation.y = facing;
            player->localTransform.position += glm::vec3(speed) * glm::vec3(deltaTime * glm::yawPitchRoll(facing, 0.0f, 0.0f) * velocity);
            
            // if player reaches the end of the road (player position in z is equal to -11), player wins
            if(player->localTransform.position.z <= -10.1) win = true;
        }

        void onCollisionEnter(Entity* entity, glm::vec3 entityCenter, glm::vec3 playerCenter){
            glm::vec3 dir = entityCenter - playerCenter;
            const float eps = 1e-3f;
            if (dir.x > eps){ right = false; entityLocking[entity].right = true;}
            else if (dir.x < -eps){ left = false; entityLocking[entity].left = true;}
            if (dir.z > eps){ backward = false; entityLocking[entity].backward = true;}
            else if (dir.z < -eps){ forward = false; entityLocking[entity].forward = true;}
            if (entity->name == "car"){
                playing = false;
            }
            collisions[entity] = true;
        }

        void onCollisionExit(Entity* entity, glm::vec3 entityCenter, glm::vec3 playerCenter){
            right ^= entityLocking[entity].right;
            left ^= entityLocking[entity].left;
            backward ^= entityLocking[entity].backward;
            forward ^= entityLocking[entity].forward;
            entityLocking[entity] = {0, 0, 0, 0};
            collisions[entity] = false;
        } 

        void exit(){
            player = nullptr;
            forward = true;
            left = true;
            right = true;
            backward = true;
            playing = true;
            win = false;
            collisions.clear();
            entityLocking.clear();
        }
    };

}
