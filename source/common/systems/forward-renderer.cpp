#include "forward-renderer.hpp"
#include "../ecs/entity.hpp"
#include "../mesh/mesh-utils.hpp"
#include "../texture/texture-utils.hpp"
#include <vector>
#include <glm/gtx/euler_angles.hpp>
#include <iostream>

namespace our
{

    void ForwardRenderer::initialize(glm::ivec2 windowSize, const nlohmann::json &config)
    {
        // First, we store the window size for later use
        this->windowSize = windowSize;
        deserialize(config);
        // Then we check if there is a sky texture in the configuration
        if (config.contains("sky"))
        {
            // First, we create a sphere which will be used to draw the sky
            this->skySphere = mesh_utils::sphere(glm::ivec2(16, 16));

            // We can draw the sky using the same shader used to draw textured objects
            ShaderProgram *skyShader = new ShaderProgram();
            skyShader->attach("assets/shaders/textured.vert", GL_VERTEX_SHADER);
            skyShader->attach("assets/shaders/textured.frag", GL_FRAGMENT_SHADER);
            skyShader->link();

            // TODO: (Req 9) Pick the correct pipeline state to draw the sky
            //  Hints: the sky will be draw after the opaque objects so we would need depth testing but which depth funtion should we pick?
            //  We will draw the sphere from the inside, so what options should we pick for the face culling.
            PipelineState skyPipelineState{};
            skyPipelineState.depthTesting.enabled = true;
            skyPipelineState.depthTesting.function = GL_LEQUAL;
            skyPipelineState.faceCulling.enabled = true;
            skyPipelineState.faceCulling.culledFace = GL_FRONT;
            skyPipelineState.faceCulling.frontFace = GL_CCW;
            skyPipelineState.setup();

            // Load the sky texture (note that we don't need mipmaps since we want to avoid any unnecessary blurring while rendering the sky)
            std::string skyTextureFile = config.value<std::string>("sky", "");
            Texture2D *skyTexture = texture_utils::loadImage(skyTextureFile, false);

            // Setup a sampler for the sky
            Sampler *skySampler = new Sampler();
            skySampler->set(GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            skySampler->set(GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            skySampler->set(GL_TEXTURE_WRAP_S, GL_REPEAT);
            skySampler->set(GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

            // Combine all the aforementioned objects (except the mesh) into a material
            this->skyMaterial = new TexturedMaterial();
            this->skyMaterial->shader = skyShader;
            this->skyMaterial->texture = skyTexture;
            this->skyMaterial->sampler = skySampler;
            this->skyMaterial->pipelineState = skyPipelineState;
            this->skyMaterial->tint = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
            this->skyMaterial->alphaThreshold = 1.0f;
            this->skyMaterial->transparent = false;
        }

        // Then we check if there is a postprocessing shader in the configuration
        if (config.contains("postprocess"))
        {
            // TODO: (Req 10) Create a framebuffer
            glGenFramebuffers(1, &postprocessFrameBuffer);
            glBindFramebuffer(GL_DRAW_FRAMEBUFFER, postprocessFrameBuffer);

            // TODO: (Req 10) Create a color and a depth texture and attach them to the framebuffer
            //  Hints: The color format can be (Red, Green, Blue and Alpha components with 8 bits for each channel).
            //  The depth format can be (Depth component with 24 bits).
            colorTarget = texture_utils::empty(GL_RGBA8, windowSize);
            depthTarget = texture_utils::empty(GL_DEPTH_COMPONENT24, windowSize);

            glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, colorTarget->getOpenGLName(), 0);
            glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthTarget->getOpenGLName(), 0);

            // TODO: (Req 10) Unbind the framebuffer just to be safe
            glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
            // Create a vertex array to use for drawing the texture
            glGenVertexArrays(1, &postProcessVertexArray);

            // Create a sampler to use for sampling the scene texture in the post processing shader
            Sampler *postprocessSampler = new Sampler();
            postprocessSampler->set(GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            postprocessSampler->set(GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            postprocessSampler->set(GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            postprocessSampler->set(GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

            // Create the post processing shader
            ShaderProgram *postprocessShader = new ShaderProgram();
            postprocessShader->attach("assets/shaders/fullscreen.vert", GL_VERTEX_SHADER);
            postprocessShader->attach(config.value<std::string>("postprocess", ""), GL_FRAGMENT_SHADER);
            postprocessShader->link();

            // Create a post processing material
            postprocessMaterial = new TexturedMaterial();
            postprocessMaterial->shader = postprocessShader;
            postprocessMaterial->texture = colorTarget;
            postprocessMaterial->sampler = postprocessSampler;
            // The default options are fine but we don't need to interact with the depth buffer
            // so it is more performant to disable the depth mask
            postprocessMaterial->pipelineState.depthMask = false;
        }

        // if (config.contains("lighted"))
        // {
        //     light->deserialize(config);

        //     ShaderProgram *lightShader = new ShaderProgram();
        //     lightShader->attach("assets/shaders/light.vert", GL_VERTEX_SHADER);
        //     lightShader->attach("assets/shaders/light.frag", GL_FRAGMENT_SHADER);
        //     lightShader->link();
        // }
    }

    void ForwardRenderer::destroy()
    {
        // Delete all objects related to the sky
        if (skyMaterial)
        {
            delete skySphere;
            delete skyMaterial->shader;
            delete skyMaterial->texture;
            delete skyMaterial->sampler;
            delete skyMaterial;
        }
        // Delete all objects related to post processing
        if (postprocessMaterial)
        {
            glDeleteFramebuffers(1, &postprocessFrameBuffer);
            glDeleteVertexArrays(1, &postProcessVertexArray);
            delete colorTarget;
            delete depthTarget;
            delete postprocessMaterial->sampler;
            delete postprocessMaterial->shader;
            delete postprocessMaterial;
        }
    }
    std::vector<Entity *> ForwardRenderer::lightedEntities(World *world)
    {
        std::vector<Entity *> lEntities;
        for (auto entity : world->getEntities())
        {
            if (auto lEntity = entity->getComponent<LightComponent>(); lEntity)
            {
                lEntities.push_back(entity);
            }
        }

        return lEntities;
    }
    std::vector<Entity *> cars(World *world)
    {
        std::vector<Entity *> lEntities;
        for (auto entity : world->getEntities())
        {
            if (auto lEntity = entity->getComponent<CarComponent>(); lEntity)
            {
                lEntities.push_back(entity);
            }
        }

        return lEntities;
    }

    void ForwardRenderer::lightSetup(std::vector<Entity *> entities, ShaderProgram *program)
    {
        program->set("light_count", (int)entities.size());
        for (int i = 0; i < (int)entities.size(); i++)
        {
            LightComponent *light = entities[i]->getComponent<LightComponent>();
            // if(!light) continue;
            program->set("lights[" + std::to_string(i) + "].type", (int)light->lightType);
            program->set("lights[" + std::to_string(i) + "].diffuse", light->diffuse);
            program->set("lights[" + std::to_string(i) + "].specular", light->specular);
            program->set("lights[" + std::to_string(i) + "].attenuation", light->attenuation);
            program->set("lights[" + std::to_string(i) + "].cone_angles", glm::vec2(glm::radians(light->cone_angles.x), glm::radians(light->cone_angles.y)));
            program->set("lights[" + std::to_string(i) + "].position", entities[i]->localTransform.position);
            glm::vec3 rotation = entities[i]->localTransform.rotation;
            program->set("lights[" + std::to_string(i) + "].direction", (glm::vec3)((glm::yawPitchRoll(rotation[1], rotation[0], rotation[2]) * (glm::vec4(0, -1, 0, 0)))));

            // std::cout << "x component of light" << light->diffuse.x << std::endl;
            // std::cout << "y component of light" << light->diffuse.y << std::endl;
            // std::cout << "z component of light" << light->diffuse.z << std::endl;
        }
    }

    void ForwardRenderer::excuteCommand(std::vector<RenderCommand> commands, glm::mat4 VP, std::vector<Entity *> lEntities, glm::vec3 eye)
    {
        for (RenderCommand command : commands)
        {
            ShaderProgram *program = command.material->shader;
            Mesh *mesh = command.mesh;
            command.material->setup();
            if(!program) return;
            program->set("eye", eye);
            program->set("M", command.localToWorld);
            program->set("MIT", glm::transpose(glm::inverse(command.localToWorld)));
            program->set("VP", VP);
            ForwardRenderer::lightSetup(lEntities, program);
            program->set("sky.top", this->sky_top);
            program->set("sky.middle", this->sky_middle);
            program->set("sky.bottom", this->sky_bottom);

            // program->set("transform", VP * command.localToWorld);
            mesh->draw();
        }
    }

    void ForwardRenderer::render(World *world)
    {
        // First of all, we search for a camera and for all the mesh renderers
        CameraComponent *camera = nullptr;
        opaqueCommands.clear();
        transparentCommands.clear();
        for (auto entity : world->getEntities())
        {
            // If we hadn't found a camera yet, we look for a camera in this entity
            if (!camera)
                camera = entity->getComponent<CameraComponent>();
            // If this entity has a mesh renderer component
            if (auto meshRenderer = entity->getComponent<MeshRendererComponent>(); meshRenderer)
            {
                // We construct a command from it
                RenderCommand command;
                command.localToWorld = meshRenderer->getOwner()->getLocalToWorldMatrix();
                command.center = glm::vec3(command.localToWorld * glm::vec4(0, 0, 0, 1));
                command.mesh = meshRenderer->mesh;
                command.material = meshRenderer->material;
                // if it is transparent, we add it to the transparent commands list
                if (command.material->transparent)
                {
                    transparentCommands.push_back(command);
                }
                else
                {
                    // Otherwise, we add it to the opaque command list
                    opaqueCommands.push_back(command);
                }
            }
            else if (auto meshRenderer = entity->getComponent<CarComponent>(); meshRenderer)
            {
                RenderCommand command;
                command.localToWorld = meshRenderer->getOwner()->getLocalToWorldMatrix();
                command.center = glm::vec3(command.localToWorld * glm::vec4(0, 0, 0, 1));
                command.mesh = meshRenderer->mesh;
                command.material = meshRenderer->material;
                // if it is transparent, we add it to the transparent commands list

                // Otherwise, we add it to the opaque command list
                opaqueCommands.push_back(command);
            }
        }

        // If there is no camera, we return (we cannot render without a camera)
        if (camera == nullptr)
            return;

        // TODO: (Req 8) Modify the following line such that "cameraForward" contains a vector pointing the camera forward direction
        // contains a vector pointing the camera forward direction
        //  HINT: See how you wrote the CameraComponent::getViewMatrix, it should help you solve this one
        glm::vec3 cameraForward = glm::vec3(0.0, 0.0, 1.0);
        std::sort(transparentCommands.begin(), transparentCommands.end(), [cameraForward](const RenderCommand &first, const RenderCommand &second)
                  {
                      // TODO: (Req 8) Finish this function
                      //  HINT: the following return should return true "first" should be drawn before "second".
                      //  The code is just an example, you should change it to return true if "first" should be drawn
                      //  before "second"

                      if ((pow(first.center.x - cameraForward.x, 2) + pow(first.center.y - cameraForward.y, 2) + pow(first.center.z - cameraForward.z, 2)) > (pow(second.center.x - cameraForward.x, 2) + pow(second.center.y - cameraForward.y, 2) + pow(second.center.z - cameraForward.z, 2)))
                          return true;
                      return false; });

        // TODO: (Req 8) Get the camera ViewProjection matrix and store it in VP
        // v matrex is for get the camera position and view
        // p is matrex is the projection materx transform
        // vp materx is for transform from cam space to the view port
        glm::mat4 VP = camera->getProjectionMatrix(windowSize) * camera->getViewMatrix();
        // TODO: (Req 8) Set the OpenGL viewport using windowSize
        // take the whole screen
        glViewport(0, 0, windowSize[0], windowSize[1]);

        // TODO: (Req 8) Set the clear color to black and the clear depth to 1
        glClearColor(0.0, 0.0, 0.0, 1.0);
        glClearDepth(1.0);

        // TODO: (Req 8) Set the color mask to true and the depth mask to true (to ensure the glClear will affect the framebuffer)
        // (to ensure the glClear will affect the framebuffer)

        glColorMask(true, true, true, true);
        glDepthMask(true);

        // If there is a postprocess material, bind the framebuffer
        if (postprocessMaterial)
        {
            // TODO: (Req 10) bind the framebuffer
            glBindFramebuffer(GL_DRAW_FRAMEBUFFER, postprocessFrameBuffer);
        }

        // TODO: (Req 8) Clear the color and depth buffers
        glClear(GL_COLOR_BUFFER_BIT);
        glClear(GL_DEPTH_BUFFER_BIT);

        std::vector<Entity *> lEntities = lightedEntities(world);
        glm::vec3 eye = glm::vec3(camera->getOwner()->getLocalToWorldMatrix() * glm::vec4(glm::vec3(0, 0, 0), 1.0f));
        //  auto executeCommands = [&VP, &camera, &lEntities, this](std::vector<RenderCommand> commands)
        //{
        // for (RenderCommand command : commands)
        // {
        //     ShaderProgram *program = command.material->shader;
        //     Mesh *mesh = command.mesh;
        //     command.material->setup();

        //     program->set("eye", glm::vec3(camera->getOwner()->getLocalToWorldMatrix() * glm::vec4(glm::vec3(0, 0, 0), 1.0f)));
        //     program->set("M", command.localToWorld);
        //     program->set("MIT", glm::transpose(glm::inverse(command.localToWorld)));
        //     program->set("VP", VP);
        //     lightSetup(lEntities, program);
        //     program->set("sky.top", this->sky_top);
        //     program->set("sky.middle", this->sky_middle);
        //     program->set("sky.bottom", this->sky_bottom);
        //     mesh->draw();
        // }
        // };

        // TODO: (Req 8) Draw all the opaque commands
        //  Don't forget to set the "transform" uniform to be equal the model-view-projection matrix for each render command

        // for (int i = 0; i < opaqueCommands.size(); i++)
        // {
        //     /* we should draw here */
        //     opaqueCommands[i].material->setup();
        //     opaqueCommands[i].material->shader->set("transform", VP * opaqueCommands[i].localToWorld);
        //     opaqueCommands[i].mesh->draw();
        // }
        // std::vector<Entity *> cEntities = cars(world);
        ForwardRenderer::excuteCommand(opaqueCommands,VP,lEntities,eye);
        // ForwardRenderer::excuteCommand(opaqueCommands,VP,cEntities,eye);

        // If there is a sky material, draw the sky
        if (this->skyMaterial)
        {
            // TODO: (Req 9) setup the sky material
            this->skyMaterial->setup();

            // TODO: (Req 9) Get the camera position

            glm::vec4 camPosition = camera->getOwner()->getLocalToWorldMatrix() * glm::vec4(0, 0, 0, 1.0f);

            // TODO: (Req 9) Create a model matrix for the sky such that it always follows the camera (sky sphere center = camera position)
            glm::mat4 transform = glm::translate(glm::mat4(1), (glm::vec3)camPosition);

            // TODO: (Req 9) We want the sky to be drawn behind everything (in NDC space, z=1)
            //  We can acheive the is by multiplying by an extra matrix after the projection but what values should we put in it?
            glm::mat4 alwaysBehindTransform = glm::mat4(
                //  Row1, Row2, Row3, Row4
                1.0f, 0.0f, 0.0f, 0.0f, // Column1
                0.0f, 1.0f, 0.0f, 0.0f, // Column2
                0.0f, 0.0f, 0.0f, 0.0f, // Column3
                0.0f, 0.0f, 1.0f, 1.0f  // Column4
            );
            // TODO: (Req 9) set the "transform" uniform

            skyMaterial->shader->set("VP", alwaysBehindTransform * VP);
            skyMaterial->shader->set("M", transform);
            // TODO: (Req 9) draw the sky sphere
            skySphere->draw();
        }
        // TODO: (Req 8) Draw all the transparent commands
        //  Don't forget to set the "transform" uniform to be equal the model-view-projection matrix for each render command

        // for (int i = 0; i < transparentCommands.size(); i++)
        // {
        //     transparentCommands[i].material->setup();
        //     transparentCommands[i].material->shader->set("transform", VP * transparentCommands[i].localToWorld);
        //     transparentCommands[i].mesh->draw();
        // }


        ForwardRenderer::excuteCommand(transparentCommands,VP,lEntities,eye);

        // If there is a postprocess material, apply postprocessing
        if (postprocessMaterial)
        {
            // TODO: (Req 10) Return to the default framebuffer
            glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);

            // TODO: (Req 10) Setup the postprocess material and draw the fullscreen triangle
            // from textured material
            postprocessMaterial->setup();
             glActiveTexture(GL_TEXTURE1);
            ShaderProgram* shader=postprocessMaterial->shader;
            depthTarget->bind();
            postprocessMaterial->sampler->bind(1);
            shader->set("depth_sampler", 1);
            shader->set("inverse_projection",glm::inverse(camera->getProjectionMatrix(windowSize)));
            glActiveTexture(GL_TEXTURE0);
            glBindVertexArray(postProcessVertexArray);
            glDrawArrays(GL_TRIANGLES, 0, 3);
        }
    }

}