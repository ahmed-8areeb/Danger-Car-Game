#include "car.hpp"
#include "../ecs/entity.hpp"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "../asset-loader.hpp"

namespace our
{
    // Reads camera parameters from the given json object
    void CarComponent::deserialize(const nlohmann::json &data)
    {
         if (!data.is_object())
            return;
    
        //cvelocity = glm::vec3(data["cvelocity"]);
       /// cvelocity = data.value("cvelocity", cvelocity);
        mesh = AssetLoader<Mesh>::get(data["mesh"].get<std::string>());
        material = AssetLoader<Material>::get(data["material"].get<std::string>());
    }

    // Creates and returns the camera view matrix
    glm::mat4 CarComponent::getViewMatrix() const
    {
        auto owner = getOwner();
        auto M = owner->getLocalToWorldMatrix();
        // TODO: (Req 7) Complete this function
        // HINT:
        //  In the camera space:
        //  - eye is the origin (0,0,0)
        //  - center is any point on the line of sight. So center can be any point (0,0,z) where z < 0. For simplicity, we let center be (0,0,-1)
        //  - up is the direction (0,1,0)
        //  but to use glm::lookAt, we need eye, center and up in the world state.
        //  Since M (see above) transforms from the camera to thw world space, you can use M to compute:
        //  - the eye position which is the point (0,0,0) but after being transformed by M
        //  - the center position which is the point (0,0,-1) but after being transformed by M
        //  - the up direction which is the vector (0,1,0) but after being transformed by M
        //  then you can use glm::lookAt

        glm::mat4 view = glm::lookAt(
            glm::vec3(M * glm::vec4(glm::vec3(0, 0, 0), 1.0f)),
            glm::vec3(M * glm::vec4(glm::vec3(0, 0, -1), 1.0f)),
            glm::vec3(M * glm::vec4(glm::vec3(0, 1, 0), 0)));

        return view;
    }

  
}