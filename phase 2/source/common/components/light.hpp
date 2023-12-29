#pragma once

#include "../ecs/component.hpp"

#include <glm/mat4x4.hpp>

namespace our {

    // An enum that defines the type of the Light
    enum class LightType {
        DIRECTIONAL,
        POINT,
        SPOT  
    };


    class LightComponent : public Component {
    public:
        //Task0
        LightType lightType; // The type of the Light
        //Directional, Point and Spot Properties
        glm::vec3 diffuse = glm::vec3(0, 0, 0);
        glm::vec3 specular = glm::vec3(0, 0, 0);
        glm::vec3 ambient = glm::vec3(1, 1, 1);
        //Point and Spot Properties
        glm::vec3 attenuation = glm::vec3(0.1, 0, 0); // vec3(quadratic, linear, constant)
        //Spot Properties
        glm::vec2 cone = glm::vec2(40, 20); // vec2(outer_angle, inner_angle)
        // The ID of this component type is "Light"
        static std::string getID() { return "Light"; }

        // Reads light parameters from the given json object
        void deserialize(const nlohmann::json& data) override;

        glm::vec3 getPosition() const;

        glm::vec3 getDirection() const;
    };

}