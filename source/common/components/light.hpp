#pragma once

#include <glm/glm.hpp>
#include "../ecs/component.hpp"
#include <glm/glm.hpp>

namespace our
{
  enum class LightType
  {
    DIRECTIONAL,
    POINT,
    SPOT
  };

  class LightComponent : public Component
  {
  public:
    LightType lightType;

    glm::vec3 diffuse = {0.0f, 0.0f, 0.0f};
    glm::vec3 specular = {0.0f, 0.0f, 0.0f};
    glm::vec3 attenuation = {1.0f, 0.0f, 0.0f};
    glm::vec2 cone_angles = {45, 90};


    // The ID of this component type is "Light"
    static std::string getID() { return "Light"; }

    // Reads camera parameters from the given json object
    void deserialize(const nlohmann::json &data) override;
  };

} // namespace our