#include "light.hpp"
#include "../deserialize-utils.hpp"
#include "../ecs/entity.hpp"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp> 

namespace our {
    // Reads light parameters from the given json object
    void LightComponent::deserialize(const nlohmann::json& data){
        //Task0
        if(!data.is_object()) return;
        std::string lightTypeStr = data.value("lightType", "directional");
        if(lightTypeStr == "directional"){
            lightType = LightType::DIRECTIONAL;
        } else if(lightTypeStr == "point") {
            lightType = LightType::POINT;
        } else {
            lightType = LightType::SPOT;
        }

        diffuse = data.value("diffuse",  diffuse);
        specular = data.value("specular", specular);
        ambient = data.value("ambient", ambient);

        attenuation = data.value("attenuation", attenuation);

        cone = data.value("cone", cone) * (glm::pi<float>() / 180);;
    }

    //Get light position from owning entity
    glm::vec3 LightComponent::getPosition() const{
        auto owner = getOwner();
        auto M = owner->getLocalToWorldMatrix();
        return M * glm::vec4(0, 0, 0, 1);
    }
    
    //Get light direction from owning entity        
    glm::vec3 LightComponent::getDirection() const{
        auto owner = getOwner();
        auto M = owner->getLocalToWorldMatrix();
        return M * glm::vec4(0, -1, 0, 0);
    }
}