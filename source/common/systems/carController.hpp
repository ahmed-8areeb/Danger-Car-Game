#pragma once
#include<iostream>
#include "../ecs/world.hpp"
#include "../components/car.hpp"

#include "../application.hpp"

#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>
#include <glm/trigonometric.hpp>
#include <glm/gtx/fast_trigonometry.hpp>


namespace our
{

    // The free camera controller system is responsible for moving every entity which contains a FreeCameraControllerComponent.
    // This system is added as a slightly complex example for how use the ECS framework to implement logic. 
    // For more information, see "common/components/free-camera-controller.hpp"
    class CarControllerSystem {
        Application* app; // The application in which the state runs
        //bool mouse_locked = false; // Is the mouse locked
    public:
        bool jumped = false;
        double seconds=1;
        double jumpTime = 0.5;
        // When a state enters, it should call this function and give it the pointer to the application
        void enter(Application* app){
            this->app = app;
        }

        // This should be called every frame to update all entities containing a FreeCameraControllerComponent 
        void update(World* world, float deltaTime) {
           
            // First of all, we search for an entity containing both a CameraComponent and a FreeCameraControllerComponent
            // As soon as we find one, we break
            CarComponent* car = nullptr;
            // FreeCameraControllerComponent *controller = nullptr;
            for(auto entity : world->getEntities()){
                car = entity->getComponent<CarComponent>();
                // controller = entity->getComponent<FreeCameraControllerComponent>();
                if(car) break;
            }
            // If there is no entity with both a CameraComponent and a FreeCameraControllerComponent, we can do nothing so we return
            if(!(car)) return;
            // Get the entity that we found via getOwner of camera (we could use controller->getOwner())
            Entity* entity = car->getOwner();

     
            // We get a reference to the entity's position and rotation
            glm::vec3& position = entity->localTransform.position;
            glm::vec3& rotation = entity->localTransform.rotation;
            glm::vec3& scale = entity->localTransform.scale;
            // glm::vec3 velocity =entity->localTransform.velocity;

            // If the left mouse button is pressed, we get the change in the mouse location
            // and use it to update the camera rotation
         

            // We prevent the pitch from exceeding a certain angle from the XZ plane to prevent gimbal locks
            if(rotation.x < -glm::half_pi<float>() * 0.99f) rotation.x = -glm::half_pi<float>() * 0.99f;
            if(rotation.x >  glm::half_pi<float>() * 0.99f) rotation.x  = glm::half_pi<float>() * 0.99f;
            // This is not necessary, but whenever the rotation goes outside the 0 to 2*PI range, we wrap it back inside.
            // This could prevent floating point error if the player rotates in single direction for an extremely long time. 
            rotation.y = glm::wrapAngle(rotation.y);

            // We update the camera fov based on the mouse wheel scrolling amount
            // float fov = camera->fovY + app->getMouse().getScrollOffset().y * controller->fovSensitivity;
            // fov = glm::clamp(fov, glm::pi<float>() * 0.01f, glm::pi<float>() * 0.99f); // We keep the fov in the range 0.01*PI to 0.99*PI
            // camera->fovY = fov;

            // We get the camera model matrix (relative to its parent) to compute the front, up and right directions
            glm::mat4 matrix = entity->localTransform.toMat4();

            glm::vec3 front = glm::vec3(matrix * glm::vec4(0, 0, -1, 0)),
                      up = glm::vec3(matrix * glm::vec4(0, 1, 0, 0)), 
                      right = glm::vec3(matrix * glm::vec4(1, 0, 0, 0));

       
            
            // A & D moves the player left or right 
            if(app->getKeyboard().isPressed(GLFW_KEY_D) && position.x>-10.0f) position += right * (deltaTime *2);
            if(app->getKeyboard().isPressed(GLFW_KEY_A)  && position.x<10.0f) position -= right * (deltaTime * 2);
            if(app->getKeyboard().isPressed(GLFW_KEY_X)) rotation.y += glm::radians(1.0f);

            if(jumped){
                seconds-=deltaTime;
                if(seconds<=0) jumped=false;
                if(jumpTime <= seconds)
                    position.y += 0.1;
                else position.y -= 0.1;
            }else if(app->getKeyboard().isPressed(GLFW_KEY_W)){
                position.y = 1.5;
                jumped = true;
                seconds = 1;
            }
        
        }

       

    };

}
