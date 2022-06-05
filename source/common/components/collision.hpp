#pragma once

#include "../ecs/component.hpp"

#include <glm/mat4x4.hpp>

namespace our
{

    // this is a component that handle the collision of the entity
    class CollisionComponent : public Component
    {
    public:
        std::string obstucaseType;
        int effect;
        
        static std::string getID() { return "Collision"; }

        void deserialize(const nlohmann::json &data) override;

        // Creates and returns the camera view matrix
        glm::mat4 getViewMatrix() const;
    };

} // namespace our
