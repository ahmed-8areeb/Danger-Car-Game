#pragma once

#include "../ecs/world.hpp"
#include "../ecs/entity.hpp"
#include "../components/camera.hpp"
#include "../components/car.hpp"
#include "../components/collision.hpp"

#include "../components/mesh-renderer.hpp"
#include "../asset-loader.hpp"
#include "../components/light.hpp"



#include <glad/gl.h>
#include <vector>
#include <algorithm>

namespace our
{
    
    // The render command stores command that tells the renderer that it should draw
    // the given mesh at the given localToWorld matrix using the given material
    // The renderer will fill this struct using the mesh renderer components
    struct RenderCommand {
        glm::mat4 localToWorld;
        glm::vec3 center;
        Mesh* mesh;
        Material* material;
    };

    // A forward renderer is a renderer that draw the object final color directly to the framebuffer
    // In other words, the fragment shader in the material should output the color that we should see on the screen
    // This is different from more complex renderers that could draw intermediate data to a framebuffer before computing the final color
    // In this project, we only need to implement a forward renderer
    class ForwardRenderer {
        // These window size will be used on multiple occasions (setting the viewport, computing the aspect ratio, etc.)
        glm::ivec2 windowSize;
        // These are two vectors in which we will store the opaque and the transparent commands.
        // We define them here (instead of being local to the "render" function) as an optimization to prevent reallocating them every frame
        std::vector<RenderCommand> opaqueCommands;
        std::vector<RenderCommand> transparentCommands;
        // Objects used for rendering a skybox
        Mesh* skySphere;
        TexturedMaterial* skyMaterial;
        // Objects used for Postprocessing
        GLuint postprocessFrameBuffer, postProcessVertexArray;
        Texture2D *colorTarget, *depthTarget;
        TexturedMaterial* postprocessMaterial;

        LightComponent * light;

        // for sky material
        glm::vec3 sky_top;
        glm::vec3 sky_middle;
        glm::vec3 sky_bottom;

    public:
        // Initialize the renderer including the sky and the Postprocessing objects.
        // windowSize is the width & height of the window (in pixels).
        void initialize(glm::ivec2 windowSize, const nlohmann::json& config);
        // Clean up the renderer
        void destroy();
        // This function should be called every frame to draw the given world
        void render(World* world);

        std::vector<Entity *> lightedEntities(World *world);
        void lightSetup(std::vector<Entity *> entities, ShaderProgram *program);
        void excuteCommand(std::vector<RenderCommand> commands,glm::mat4 VP,std::vector<Entity *> lEntities,glm::vec3 eye);
        void deserialize(const nlohmann::json &data) 
        {
            if (data.contains("sky_top"))
            {
                std::vector<float> v=data["sky_top"];
                sky_top = glm::vec3(v[0], v[1], v[2]);
            }
            if(data.contains("sky_middle"))
            {
                std::vector<float> v=data["sky_middle"];
                sky_middle = glm::vec3(v[0], v[1], v[2]);
            }
            if(data.contains("sky_bottom"))
            {
                std::vector<float> v=data["sky_bottom"];
                sky_bottom = glm::vec3(v[0], v[1], v[2]);
            }
            
        }
    };

}