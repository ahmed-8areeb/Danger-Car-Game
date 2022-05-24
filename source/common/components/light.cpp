#include "light.hpp"
#include "../deserialize-utils.hpp"
namespace our
{

  
  // Reads camera parameters from the given json object
  void LightComponent::deserialize(const nlohmann::json &data)
  {
    if (!data.is_object())
      return;
    std::string lightTypeStr = data.value("lightType", "directional");
    
    if (lightTypeStr == "directional")
    {
      lightType = LightType::DIRECTIONAL;
    }
    else if (lightTypeStr == "point")
    {
      lightType = LightType::POINT;
      attenuation = data.value("attenuation", attenuation);
    }
    else if (lightTypeStr == "spot")
    {
      lightType = LightType::SPOT;
      cone_angles = data.value("cone_angles", cone_angles);
    }

    diffuse = data.value("diffuse", diffuse);
    specular = data.value("specular", specular);
  }
}